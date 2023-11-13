//
// Created by elliot on 16/10/23.
//

#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include <tuple>
#include <algorithm>
#include <map>
#include "ModelTriangle.h"
#include "TextureMap.h"
#include "Triangle.h"

using namespace std;
using namespace glm;
using MaybeTexture = pair<TextureMap, bool>; //a texture and its validity

class ModelLoader {
public:
    explicit ModelLoader(string fileName, float scale, glm::vec3 position);
//    ~ModelLoader(); // delete the byte string and vector of model triangles

    //loads the file and returns the string
    void load();

    // shows all tris coords
    void printTris();

    vector<Triangle*> getTris();

    glm::vec3 getPos();
private:

    glm::vec3 position;
    std::vector<string> tailTokens(std::vector<string> ln, const string& tkn);
    bool isLineType(std::vector<string> ln, const string& tkn);
    string fileName; // name
    string bytes; // file bytes
    vector<Triangle*> tris; //tris generated from verts and facets
    map<string, Colour> materials; //vector of colour maps
    map<std::string, TextureMap> textures;
    float scale;

    static const string TKN_MTLLIB;
    void asMaterial(vector<string> ln);
    static const string TKN_SUBOBJ;
    static const string TKN_USEMTL;
    void asUseMaterial(vector<string> ln, Colour &currentColour, MaybeTexture &currentTexture);
    static const string TKN_VERTEX;
    void asVertex(vector<string> ln, vector<vec3> &verts);
    static const string TKN_FACET;
    void asFacet(vector<string> ln, vector<vec3> &verts, vector<vec2>& textureVerts, Colour &currentColour, MaybeTexture &currentTexture);
    static const string TKN_NEWMTL;
    static const string TKN_KD;
    static const string TKN_COMMNT;
    static const string TKN_TXTURE;

    vector<string> toTokens(string &lnBlock, char splitOn);

    static const string TKN_VTXTEX;

    void asVertexTexture(vector<string> ln, vector<vec2> &textureVerts);
};

