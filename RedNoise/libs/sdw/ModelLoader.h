//
// Created by elliot on 16/10/23.
//

#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <tuple>
#include <algorithm>
#include <map>
#include "ModelTriangle.h"

using namespace std;
using namespace glm;

class ModelLoader {
public:
    explicit ModelLoader(string fileName, float scale);
//    ~ModelLoader(); // delete the byte string and vector of model triangles

    //loads the file and returns the string
    void load();

    // shows all tris coords
    void printTris();

    vector<ModelTriangle> getTris();
private:
    string afterToken(string& ln, const string& tkn);
    bool isToken(string& ln, const string& tkn);
    string fileName; // name
    string bytes; // file bytes
    vector<ModelTriangle> tris; //tris generated from verts and facets
    map<string, Colour> materials; //vector of colour maps
    float scale;
};

