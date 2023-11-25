//
// Created by elliot on 16/10/23.
//

#include "ModelLoader.h"
#include "Utils.h"
#include "CanvasTriangle.h"
#include "Triangle.h"
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;
using namespace glm;

const string ModelLoader::TKN_MTLLIB = "mtllib";
const string ModelLoader::TKN_SUBOBJ = "o";
const string ModelLoader::TKN_USEMTL = "usemtl";
const string ModelLoader::TKN_VERTEX = "v";
const string ModelLoader::TKN_VTXTEX = "vt";
const string ModelLoader::TKN_FACET  = "f";
const string ModelLoader::TKN_NEWMTL = "newmtl";
const string ModelLoader::TKN_KD     = "Kd";
const string ModelLoader::TKN_COMMNT = "#";
const string ModelLoader::TKN_TXTURE = "map_Kd";
const float ModelLoader::LARGE = 10000;
const float ModelLoader::SMALL = -10000;

ModelLoader::ModelLoader(string fileName, float scale, glm::vec3 position, int shading) {
    this->scale = scale; //scaling factor
    this->fileName = fileName;
    this->bytes = ""; //new string
    this->tris = vector<Triangle*>{};
    this->verts = std::vector<glm::vec3>{};

    this->vertToTris = std::vector<std::vector<Triangle*>>{}; //lookup table for finding the tris using a vertex (index of verts)
    this->triToVerts = std::vector<std::vector<int>>{}; //the indices of verts that tri[i] is made from
    this->position = position;
    this->shading = shading;
}
//ModelLoader::~ModelLoader(){
//    delete this->tris;
//}

// just returns non-empty strings
// splits on space character blocks ' ' of any length
std::vector<string> ModelLoader::toTokens(string& lnBlock, char splitOn=' '){
    std::vector<string> canEmptyTkns = Utils::split(lnBlock, splitOn);
    std::vector<string> noEmptyTkns = {};
    for(string& tkn : canEmptyTkns){
        if(!tkn.empty()){
            noEmptyTkns.push_back(tkn);
        }
    }
    return noEmptyTkns;
}

bool ModelLoader::isLineType(std::vector<string> ln, const string& tkn){
    if(ln.empty()) return false; //no tokens?
    return ln[0] == tkn;
}

std::vector<string> ModelLoader::tailTokens(std::vector<string> ln, const string& tkn){
    if(ln.empty()) throw std::runtime_error("ModelLoader::tailTokens: there is no tail to a line with no tokens");
    //single space accounted for via +1
    return {ln.begin() + 1, ln.end()}; //chuck away the first element
}

//void ModelLoader::colourOnMaterial

//makes the necessary changes assuming line is of type mtllib
void ModelLoader::asMaterial(std::vector<string> ln){
    std::vector<string> tkns = tailTokens(ln, TKN_MTLLIB);
    const string NO_TEXTURE = "none";
    string filename = tkns.back();
    string materialBytes = Utils::fileAsString(filename);
    vector<string> materialLines = Utils::split(materialBytes, '\n');
    std::string lastName = NO_TEXTURE;
    for(size_t i = 0; i < materialLines.size() - 1; i++){
        std::vector<string> mtlLn = toTokens(materialLines[i]);
        if(isLineType(mtlLn, TKN_NEWMTL)){
            std::vector<string> lnKd = toTokens(materialLines[i + 1]);
            string name = tailTokens(mtlLn, TKN_NEWMTL).back(); //.back used to get last token for a nameline
            vector<string> channels =  tailTokens(lnKd, TKN_KD);
            vector<int> chVals = {};
            for(string& ch: channels){
                float chVal = stof(ch) * 255;
                chVals.push_back(static_cast<int>(floor(chVal)));
            }
            if(chVals.size() != 3) throw runtime_error("ModelLoader::load(): TKN_MTLLIB colour does not have three channels");
            std::cout << "colour added '" << name << "'" << std::endl;
            this->materials.insert(pair<string, Colour>(name, Colour(name, chVals[0], chVals[1], chVals[2])));
            lastName = name;
        }
        if(isLineType(mtlLn, TKN_TXTURE) && lastName != NO_TEXTURE){
            string fName = tailTokens(mtlLn, TKN_NEWMTL).back();
            std::cout << "loading texture '" << lastName << "' in file '" << fName << "'";
            this->textures.insert(pair<string, TextureMap>(lastName, TextureMap(fName)));
        }
    }
}
//look up colour in material map and read
//throws a runtime error if there is no colour with that name
void ModelLoader::asUseMaterial(std::vector<string> ln, Colour& currentColour, MaybeTexture &currentTexture){
    string name = tailTokens(ln, TKN_USEMTL).back(); //.back used to get last token for a usemtl line
    if(this->materials.count(name) > 0){
        currentColour = this->materials[name];
    }else{
        throw runtime_error("ModelLoader::asUseMaterial: invalid colour name '" + name + "'");
    }
    if(this->textures.count(name) > 0){
        currentTexture = pair<TextureMap, bool>(this->textures[name], true);
    }else{
        currentTexture = pair<TextureMap, bool>(TextureMap(), false); //there is no texture corresponding to this name
    }

}

//process tokens as a string of three floats
void ModelLoader::asVertex(std::vector<string> ln, vector<vec3>& verts){
    std::vector<string> floatsLn = tailTokens(ln, TKN_VERTEX);
    vector<float> floats;
    for(const string& flt: floatsLn){
        try{
            floats.push_back(stof(flt));
        }catch(invalid_argument& err){ //failed to read as float
            std::cout << "warning, reading as float failed for '" << flt << "':" << err.what() << std::endl;
            floats.push_back(static_cast<float>(stoi(flt)));
        }
    }
    if(floats.size() != 3) throw runtime_error("ModelLoader::load(): TKN_VERTEX conversion resulted in the wrong number of floats");
    verts.push_back(vec3(floats[0], floats[1], floats[2]) * this->scale);
    this->vertToTris.push_back({}); //empty list to be populated with triangles
}

void ModelLoader::asVertexTexture(std::vector<string> ln, vector<vec2>& textureVerts){
    std::vector<string> floatsLn = tailTokens(ln, TKN_VTXTEX); //LOOKS LIKE vt x x
    vector<float> floats;                                          //           where x is a float
    for(const string& flt: floatsLn){
        try{
            floats.push_back(stof(flt));
        }catch(invalid_argument& err){ //failed to read as float
            std::cout << "warning, reading as float failed for '" << flt << "':" << err.what() << std::endl;
            floats.push_back(static_cast<float>(stoi(flt)));
        }
    }
    //two floats
    if(floats.size() != 2) throw runtime_error("ModelLoader::load(): TKN_VTXTEX conversion resulted in the wrong number of floats");
    textureVerts.push_back(glm::vec2(floats[0], floats[1]));
}


void ModelLoader::asFacet(std::vector<string> ln, vector<vec3>& verts, vector<vec2>& textureVerts, Colour& currentColour, MaybeTexture& currentTexture){
    vector<string> facetsLn = tailTokens(ln, TKN_FACET);
    vector<int> facetVertsIndices = {};
    vector<int> textureVertsIndices = {}; //stays empty if no valid texture is present
    size_t VERT_TEXTURE_LEN = 2;
    size_t VERT_COUNT = 3;
    for(string& fct: facetsLn){
        std::vector<string> vertMaybeTexture = toTokens(fct, '/'); //vert and potential texture coord
        bool hasTextureVert = vertMaybeTexture.size() == VERT_TEXTURE_LEN;
        //for(string s : vertMaybeTexture) std::cout << "'" << s << "'" << endl;
        if(hasTextureVert && !currentTexture.second) //there is a texture vertex but no valid current texture
            throw runtime_error("ModelLoader::asFacet: facet has a texture vertex but no valid texture");
        if(!hasTextureVert && currentTexture.second)
            throw runtime_error("ModelLoader::asFacet: facet has no texture vertex but a valid texture");
        facetVertsIndices.push_back(stoi(vertMaybeTexture[0]));
        if(hasTextureVert) textureVertsIndices.push_back(stoi(vertMaybeTexture[1]));
    }
    if(facetVertsIndices.size() != VERT_COUNT) throw runtime_error("ModelLoader::load(): TKN_FACET facet does not have three vertices");
    if(textureVertsIndices.size() != VERT_COUNT && !textureVertsIndices.empty()) throw runtime_error("ModelLoader::load(): invalid number of texture coordinates (not none, not three)");
    //create a triangle
    int i0 = facetVertsIndices[0] - 1;
    int i1 = facetVertsIndices[1] - 1;
    int i2 = facetVertsIndices[2] - 1; //indices of vertices
    vec3 v0 = verts[i0] - this->vertCentre;
    vec3 v1 = verts[i1] - this->vertCentre;
    vec3 v2 = verts[i2] - this->vertCentre;
    Triangle* tri;
    if(currentTexture.second){ //do we have a valid texture mapping
        TextureMap texture = currentTexture.first;
        size_t w = texture.width;
        size_t h = texture.height;
        glm::vec2 vt0 = textureVerts[textureVertsIndices[0] - 1];
        glm::vec2 vt1 = textureVerts[textureVertsIndices[1] - 1];
        glm::vec2 vt2 = textureVerts[textureVertsIndices[2] - 1];
        CanvasPoint scaledVt0 = *new CanvasPoint(vt0.x * static_cast<float>(w), vt0.y * static_cast<float>(h));
        CanvasPoint scaledVt1 = *new CanvasPoint(vt1.x * static_cast<float>(w), vt1.y * static_cast<float>(h));
        CanvasPoint scaledVt2 = *new CanvasPoint(vt2.x * static_cast<float>(w), vt2.y * static_cast<float>(h));
        CanvasTriangle textureTri = *new CanvasTriangle(scaledVt0, scaledVt1, scaledVt2);
        tri = new Triangle(v0, v1, v2, currentColour, texture, textureTri);
    }else tri = new Triangle(v0, v1, v2, currentColour);

    this->tris.push_back(tri);
    this->triToVerts.push_back(std::vector<int>{i0, i1, i2});
    this->vertToTris[i0].push_back(tri);
    this->vertToTris[i1].push_back(tri);
    this->vertToTris[i2].push_back(tri);
}

// main object loading function
void ModelLoader::load() {
    this->bytes = Utils::fileAsString(this->fileName);
    std::vector<string> lines = Utils::split(this->bytes, '\n');
    //tokens

    Colour currentColour = Colour(255, 0, 0);
    MaybeTexture currentTexture = pair<TextureMap, bool>{TextureMap(), false}; //texture map and validity
    std::vector<vec2> textureVerts = {};

    //average the vertex positions first
    for(string& lnBlock: lines) {
        std::vector<string> ln = toTokens(lnBlock);
        if(!isLineType(ln, TKN_COMMNT) && isLineType(ln, TKN_VERTEX)) asVertex(ln, this->verts);
    }
    this->boundVertices(); //how much we offset the model by

    for(string& lnBlock: lines){
        std::vector<string> ln = toTokens(lnBlock);
        if(!isLineType(ln, TKN_COMMNT)){
            if(isLineType(ln, TKN_MTLLIB)) asMaterial(ln);
            if(isLineType(ln, TKN_SUBOBJ)){} //add sub object names if needed
            if(isLineType(ln, TKN_USEMTL)) asUseMaterial(ln, currentColour, currentTexture);
            //if(isLineType(ln, TKN_VERTEX)) asVertex(ln, this->verts); // we already load in the vertices
            if(isLineType(ln, TKN_VTXTEX)) asVertexTexture(ln, textureVerts);
            if(isLineType(ln, TKN_FACET)) asFacet(ln, this->verts, textureVerts, currentColour, currentTexture);
        }
    }
    this->makeVertexNorms();
    //this->putVertNormsInTris();
}

void ModelLoader::makeVertexNorms() {
    std::vector<glm::vec3> vNorms = {};
    // average all the normals of all the triangles connected to each vertex
    for(size_t i = 0; i < this->verts.size(); i++){
        //all the triangles connected to each vertex
        std::vector<Triangle*> sharedTris = this->vertToTris[i];
        glm::vec3 normSum = {0.0, 0.0, 0.0};
        for(Triangle* & sharedTri : sharedTris){
            normSum = normSum + *sharedTri->getNormal();
        }
        normSum = glm::normalize(normSum); //the vertex normal
        this->vertNorms.push_back(normSum);
    }
}

void ModelLoader::boundVertices(){
    glm::vec3 vertSum = {0, 0, 0};
    glm::vec3 boundsMin = {LARGE, LARGE, LARGE};
    glm::vec3 boundsMax = {SMALL, SMALL, SMALL};
    for(glm::vec3 v : this->verts){
        boundsMin.x = glm::min(v.x, boundsMin.x);
        boundsMin.y = glm::min(v.y, boundsMin.y);
        boundsMin.z = glm::min(v.z, boundsMin.z);
        boundsMax.x = glm::max(v.x, boundsMax.x);
        boundsMax.y = glm::max(v.y, boundsMax.y);
        boundsMax.z = glm::max(v.z, boundsMax.z);
    }
    glm::vec3 boundsCentre = (boundsMin + boundsMax) / static_cast<float>(2);
    //the centre of this bounding box is independent of the detail concentration on the model
    this->vertCentre = boundsCentre; // the relative centre of the vertices
//    for(int i = 0; i < this->verts.size(); i++){
//        this->verts[i] = this->verts[i] - this->vertCentre;
//
//    }
}

//
//void ModelLoader::putVertNormsInTris(){
//    for(size_t i = 0; i < this->tris.size(); i++){
//        std::vector<int> triVertIndices = this->triToVerts[i];
//        int v0i = triVertIndices[0];
//        int v1i = triVertIndices[1];
//        int v2i = triVertIndices[2];
//        this->tris[i]->setN0(this->vertNorms[v0i]);
//        this->tris[i]->setN1(this->vertNorms[v1i]);
//        this->tris[i]->setN2(this->vertNorms[v2i]);
//    }
//}

std::vector<glm::vec3*> ModelLoader::getNormsForTri(int& triIndex) {
    std::vector<int> triVertIndices = this->triToVerts[triIndex];
    int v0i = triVertIndices[0];
    int v1i = triVertIndices[1];
    int v2i = triVertIndices[2];
    return {&this->vertNorms[v0i], &this->vertNorms[v1i], &this->vertNorms[v2i]};
}

glm::vec3* ModelLoader::getPos(){
    return &this->position;
}

void ModelLoader::setPos(glm::vec3 pos) {
    this->position = pos;
}

void ModelLoader::printTris() {
//    for(Triangle& tri : tris){
//        cout << tri << endl;
//        cout << tri.colour << endl;
//    }
}

vector<Triangle*> ModelLoader::getTris() {
    return this->tris;
}

int* ModelLoader::getShading() {
    return &this->shading;
}



//vector<glm::vec3> ModelLoader::makeVertexNorms() {
//
//}