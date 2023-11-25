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

using MaybeTexture = std::pair<TextureMap, bool>; //a texture and its validity

class ModelLoader {
public:
    explicit ModelLoader(std::string fileName, float scale, glm::vec3 position, int shading);
//    ~ModelLoader(); // delete the byte string and vector of model triangles

    //loads the file and returns the string
    void load();

    // shows all tris coords
    void printTris();

    std::vector<Triangle*> getTris();

    void setPos(glm::vec3 pos);
    glm::vec3* getPos();
    std::vector<glm::vec3*> getNormsForTri(int& triIndex);
    //shading mode
    enum Shading {nrm, grd, phg, mrr, phg_mrr};// normal(s), gouraud, phong, mirror, phong_mirror
    int* getShading();

    glm::vec3 getCentre();

private:
    glm::vec3 position;
    std::vector<std::string> tailTokens(std::vector<std::string> ln, const std::string& tkn);
    bool isLineType(std::vector<std::string> ln, const std::string& tkn);
    int shading;
    std::string fileName; // name
    std::string bytes; // file bytes
    std::vector<Triangle*> tris; //tris generated from verts and facets
    std::map<std::string, Colour> materials; //vector of colour maps
    std::map<std::string, TextureMap> textures;
    float scale;
    std::vector<glm::vec3> verts; //just those vertices which are used to build facets
    glm::vec3 vertCentre; // mean of all vertex locations
    std::vector<glm::vec3> vertNorms; //one to one correspondance to the verts list
    std::vector<std::vector<Triangle*>> vertToTris; //a lookup for which triangles use the ith vert
    std::vector<std::vector<int>> triToVerts; //a lookup for which vertices(indices) are used by the ith triangle in tris

    static const std::string TKN_MTLLIB;
    void asMaterial(std::vector<std::string> ln);
    static const std::string TKN_SUBOBJ;
    static const std::string TKN_USEMTL;
    void asUseMaterial(std::vector<std::string> ln, Colour &currentColour, MaybeTexture &currentTexture);
    static const std::string TKN_VERTEX;
    void asVertex(std::vector<std::string> ln, std::vector<glm::vec3> &verts);
    static const std::string TKN_FACET;
    void asFacet(std::vector<std::string> ln, std::vector<glm::vec3> &verts, std::vector<glm::vec2>& textureVerts, Colour &currentColour, MaybeTexture &currentTexture);
    static const std::string TKN_NEWMTL;
    static const std::string TKN_KD;
    static const std::string TKN_COMMNT;
    static const std::string TKN_TXTURE;

    std::vector<std::string> toTokens(std::string &lnBlock, char splitOn);

    static const std::string TKN_VTXTEX;

    void asVertexTexture(std::vector<std::string> ln, std::vector<glm::vec2> &textureVerts);

    void makeVertexNorms(); //compute all vertex normals through finding all unique vertices, and seeing which triangles share them
    //void putVertNormsInTris();
    void averageVertexPos();

};

