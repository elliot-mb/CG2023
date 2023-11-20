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
    explicit ModelLoader(string fileName, float scale, glm::vec3 position, int shading);
//    ~ModelLoader(); // delete the byte string and vector of model triangles

    //loads the file and returns the string
    void load();

    // shows all tris coords
    void printTris();

    vector<Triangle*> getTris();

    glm::vec3 getPos();
    std::vector<glm::vec3*> getNormsForTri(int& triIndex);
    //shading mode
    enum Shading {nrm, grd, phg};// normal(s), gouraud, phong
    int* getShading();
private:
    glm::vec3 position;
    std::vector<string> tailTokens(std::vector<string> ln, const string& tkn);
    bool isLineType(std::vector<string> ln, const string& tkn);
    int shading;
    string fileName; // name
    string bytes; // file bytes
    vector<Triangle*> tris; //tris generated from verts and facets
    map<string, Colour> materials; //vector of colour maps
    map<std::string, TextureMap> textures;
    float scale;
    std::vector<glm::vec3> verts; //just those vertices which are used to build facets
    std::vector<glm::vec3> vertNorms; //one to one correspondance to the verts list
    std::vector<std::vector<Triangle*>> vertToTris; //a lookup for which triangles use the ith vert
    std::vector<std::vector<int>> triToVerts; //a lookup for which vertices(indices) are used by the ith triangle in tris

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

    void makeVertexNorms(); //compute all vertex normals through finding all unique vertices, and seeing which triangles share them
    //void putVertNormsInTris();
};

