//
// Created by elliot on 16/10/23.
//

#include "ModelLoader.h"
#include "Utils.h"
#include "CanvasTriangle.h"
#include <iostream>
#include <fstream>
#include <string.h>

ModelLoader::ModelLoader(string fileName, float scale, glm::vec3 position) {
    this->scale = scale; //scaling factor
    this->fileName = fileName;
    this->bytes = ""; //new string
    this->tris = vector<ModelTriangle>{}; //new modeltriangle vector
    this->position = position;
}
//ModelLoader::~ModelLoader(){
//    delete this->tris;
//}

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

void ModelLoader::load() {
    this->bytes = Utils::fileAsString(this->fileName);
    std::vector<string> lines = Utils::split(this->bytes, '\n');
    //tokens
    const string TKN_MTLLIB = "mtllib";
    const string TKN_SUBOBJ = "o";
    const string TKN_USEMTL = "usemtl";
    const string TKN_VECTOR = "v";
    const string TKN_FACET  = "f";
    const string TKN_NEWMTL = "newmtl";
    const string TKN_KD     = "Kd";

    Colour currentColour = Colour(255, 255, 255);
    vector<vec3> verts = {};

    for(string& lnBlock: lines){
        std::vector<string> ln = Utils::split(lnBlock, ' ');
        for(const string& tkn : ln){
            cout << "_" << tkn << "";
        }
        cout << endl;
        if(isLineType(ln, TKN_MTLLIB)){
            std::vector<string> tkns =  tailTokens(ln, TKN_MTLLIB);
            string filename = tkns.back();
            string materialBytes = Utils::fileAsString(filename);
            vector<string> materialLines = Utils::split(materialBytes, '\n');
            for(size_t i = 0; i < materialLines.size() - 1; i++){
                std::vector<string> mtlLn = Utils::split( materialLines[i], ' ');
                std::vector<string> mtlLnNext = Utils::split(materialLines[i + 1], ' ');
                if(isLineType(mtlLn, TKN_NEWMTL)){
                    string name = tailTokens(mtlLn, TKN_NEWMTL).back(); //.back used to get last token for a nameline
                    vector<string> channels =  tailTokens(mtlLnNext, TKN_KD);
                    vector<int> chVals = {};
                    for(string& ch: channels){
                        float chVal = stof(ch) * 255;
                        chVals.push_back(static_cast<int>(floor(chVal)));
                    }
                    if(chVals.size() != 3) throw runtime_error("ModelLoader::load(): TKN_MTLLIB colour does not have three channels");
                    this->materials.insert(pair<string, Colour>(name, Colour(name, chVals[0], chVals[1], chVals[2])));
                }
            }
        }
        if(isLineType(ln, TKN_SUBOBJ)){
            //add sub object names if needed
        }
        if(isLineType(ln, TKN_USEMTL)){
            //look up colour in material map and read
            string name = tailTokens(ln, TKN_USEMTL).back(); //.back used to get last token for a usemtl line
            currentColour = this->materials[name];
        }
        if(isLineType(ln, TKN_VECTOR)){
            //the string of three floats
            std::vector<string> floatsLn = tailTokens(ln, TKN_VECTOR);
            vector<float> floats;
            for(const string& flt: floatsLn){
                try{
                    floats.push_back(stof(flt));
                }catch(invalid_argument& err){ //failed to read as float
                    std::cout << "warning, reading as float failed for '" << flt << "':" << err.what() << std::endl;
                    floats.push_back(static_cast<float>(stoi(flt)));
                }
            }
            if(floats.size() != 3) throw runtime_error("ModelLoader::load(): TKN_VECTOR conversion resulted in the wrong number of floats");
            verts.push_back(vec3(floats[0], floats[1], floats[2]) * scale);
        }
        if(isLineType(ln, TKN_FACET)){
            vector<string> facetsLn = tailTokens(ln, TKN_FACET);
            vector<int> facets;
            for(const string& fct: facetsLn){
                string num = Utils::split(fct, '/')[0];
                facets.push_back(stoi(num));
            }
            if(facets.size() != 3) throw runtime_error("ModelLoader::load(): TKN_FACET facet does not have three vertices");
            //create a triangle
            vec3 v0 = verts[facets[0] - 1] + this->position;
            vec3 v1 = verts[facets[1] - 1] + this->position;
            vec3 v2 = verts[facets[2] - 1] + this->position;
            this->tris.push_back(*new ModelTriangle(v0, v1, v2, currentColour));
        }
    }

}

glm::vec3 ModelLoader::getPos(){
    return this->position;
}

void ModelLoader::printTris() {
    for(ModelTriangle& tri : tris){
        cout << tri << endl;
        cout << tri.colour << endl;
    }
}

vector<ModelTriangle> ModelLoader::getTris() {
    return this->tris;
}
