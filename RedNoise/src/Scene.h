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
#include "Light.h"
#include "EnvMap.h"


class Scene {
public:
    Scene(std::vector<ModelLoader *> &models, std::vector<Light> lights, EnvMap env); //just a collection of models (world-like, always at world origin)
//    Scene(const Scene&) = delete;
    std::vector<Triangle*> getTris(); //gets the tris of all models

    ModelLoader* getModel(int modelIndex); //gets a specific model

    int getModelFromTri(int triIndex); //gets a specific model index from a specific triangle index inside this->alltris

    int getModelOffset(int modelIndex);

    glm::vec3* getModelPosition(int modelIndex);

    void setModelPosition(int modelIndex, glm::vec3 pos);

    Light& getLight(int i);

    std::vector<glm::vec3> getLightLocs();

    std::vector<float>& getLightStrengths();

    std::vector<float> getInitBrightnesses();

    std::vector<float> getInitSpeculars();

    int getNumLights();
private:
    //delegates to all models
    void load();

    std::vector<ModelLoader*> models;
    std::vector<Triangle*> allTris; //tris of all models for raycasting into
    std::vector<int> toModel;
    std::vector<int> modelOffset;
    std::vector<Light> lights;
    std::vector<glm::vec3> lightLocs;
    std::vector<float> lightStrengths;
    std::vector<float> initBrightnesses;
    std::vector<float> initSpeculars;
    EnvMap env;
    int numLights;


};


