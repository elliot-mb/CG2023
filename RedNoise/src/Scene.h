//
// Created by elliot on 21/11/23.
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
#include "ModelLoader.h"


class Scene {
public:
    Scene(std::vector<ModelLoader *> models); //just a collection of models (world-like, always at world origin)

    std::vector<Triangle*> getTris(); //gets the tris of all models

    ModelLoader* getModel(int modelIndex); //gets a specific model

    int getModelFromTri(int triIndex); //gets a specific model index from a specific triangle index inside this->alltris

    int getModelOffset(int modelIndex);

    glm::vec3* getModelPosition(int modelIndex);
    void setModelPosition(int modelIndex, glm::vec3 pos);
private:
    //delegates to all models
    void load();

    std::vector<ModelLoader*> models;
    std::vector<Triangle*> allTris; //tris of all models for raycasting into
    std::vector<int> toModel;
    std::vector<int> modelOffset;
    glm::vec3 lights;
};


