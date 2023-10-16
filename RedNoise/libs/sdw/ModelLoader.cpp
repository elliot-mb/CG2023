//
// Created by elliot on 16/10/23.
//

#include "ModelLoader.h"
#include "Utils.h"
#include <iostream>
#include <fstream>

ModelLoader::ModelLoader(string fileName, float scale) {
    this->scale = scale; //scaling factor
    this->fileName = fileName;
    this->bytes = ""; //new string
    this->tris = vector<ModelTriangle>{}; //new modeltriangle vector
}
//ModelLoader::~ModelLoader(){
//    delete this->tris;
//}

//tokens are taken as the very first thing on a line
bool ModelLoader::isToken(string& ln, const string& tkn){
    return ln.substr(0, tkn.length()) == tkn;
}

string ModelLoader::afterToken(string& ln, const string& tkn){
    //single space accounted for via +1
    return ln.substr(tkn.length() + 1, ln.length() - (tkn.length() + 1));
}

//void ModelLoader::colourOnMaterial

void ModelLoader::load() {
    this->bytes = Utils::fileAsString(this->fileName);
    vector<string> lines = Utils::split(this->bytes, '\n');
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

    for(string& ln: lines){
        if(isToken(ln, TKN_MTLLIB)){
            string filename = afterToken(ln, TKN_MTLLIB);
            string materialBytes = Utils::fileAsString(filename);
            vector<string> materialLines = Utils::split(materialBytes, '\n');
            for(size_t i = 0; i < materialLines.size(); i++){
                string mtlLn = materialLines[i];
                if(isToken(mtlLn, TKN_NEWMTL)){
                    string name = afterToken(mtlLn, TKN_NEWMTL);
                    string colour = afterToken(materialLines[i + 1], TKN_KD);
                    vector<string> channels = Utils::split(colour, ' ');
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
        if(isToken(ln, TKN_SUBOBJ)){
            //add sub object names if needed
        }
        if(isToken(ln, TKN_USEMTL)){
            //look up colour in material map and read
            string name = afterToken(ln, TKN_USEMTL);
            currentColour = this->materials[name];
        }
        if(isToken(ln, TKN_VECTOR)){
            //the string of three floats
            string floatLine = afterToken(ln, TKN_VECTOR);
            vector<string> floatsStr = Utils::split(floatLine, ' ');
            vector<float> floats;
            for(const string& flt: floatsStr){
                floats.push_back(stof(flt));
            }
            if(floats.size() != 3) throw runtime_error("ModelLoader::load(): TKN_VECTOR conversion resulted in the wrong number of floats");
            verts.push_back(vec3(floats[0], floats[1], floats[2]) * scale);
        }
        if(isToken(ln, TKN_FACET)){
            string facetLine = afterToken(ln, TKN_FACET);
            vector<string> facetsStr = Utils::split(facetLine, ' ');
            vector<int> facets;
            for(const string& fct: facetsStr){
                string num = Utils::split(fct, '/')[0];
                facets.push_back(stoi(num));
            }
            if(facets.size() != 3) throw runtime_error("ModelLoader::load(): TKN_FACET facet does not have three vertices");
            //create a triangle
            vec3 v0 = verts[facets[0] - 1];
            vec3 v1 = verts[facets[1] - 1];
            vec3 v2 = verts[facets[2] - 1];
            this->tris.push_back(*new ModelTriangle(v0, v1, v2, currentColour));
        }
    }

}

void ModelLoader::printTris() {
    for(ModelTriangle& tri : tris){
        cout << tri << endl;
        cout << tri.colour << endl;
    }
}