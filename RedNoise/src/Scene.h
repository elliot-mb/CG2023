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

    //delegates to all models
    void load();

    std::vector<Triangle*> getTris(); //gets the tris of all models

    ModelLoader& getModel(int num); //gets a specific model

private:
    std::vector<ModelLoader*> models;
    std::vector<Triangle*> allTris; //tris of all models for raycasting into
};


