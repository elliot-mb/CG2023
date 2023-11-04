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

using namespace std;
using namespace glm;

class ModelLoader {
public:
    explicit ModelLoader(string fileName, float scale, glm::vec3 position);
//    ~ModelLoader(); // delete the byte string and vector of model triangles

    //loads the file and returns the string
    void load();

    // shows all tris coords
    void printTris();

    vector<ModelTriangle> getTris();

    glm::vec3 getPos();
private:
    glm::vec3 position;
    std::vector<string> toTokens(string& lnBlock);
    std::vector<string> tailTokens(std::vector<string> ln, const string& tkn);
    bool isLineType(std::vector<string> ln, const string& tkn);
    string fileName; // name
    string bytes; // file bytes
    vector<ModelTriangle> tris; //tris generated from verts and facets
    map<string, Colour> materials; //vector of colour maps
    float scale;
};

