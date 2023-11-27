//
// Created by elliot on 21/11/23.
//

#include "Scene.h"

#include <utility>

Scene::Scene(std::vector<ModelLoader *> models, std::vector<glm::vec4*> lights) {
    this->models = std::move(models);
    this->allTris = {};
    this->toModel = {};
    this->modelOffset = {}; //the offset each model's triangles have in the allTris list
    this->lights = std::move(lights);
    load(); //private because we should really only do it once
    this->lightLocs = {};
    this->lightStrengths = {};
    this->initBrightnesses = {};
    this->initSpeculars = {};
    for(glm::vec4* loc : this->lights){
        lightLocs.push_back(glm::vec3(*loc));
        lightStrengths.push_back(&loc->w); //address of the ws
        initBrightnesses.push_back(1.0);
        initSpeculars.push_back(0.0);
    }
    this->numLights = static_cast<int>(this->lights.size());
}

void Scene::load() {
    int modelIndex = 0;
    for(ModelLoader* m : this->models){
        this->modelOffset.push_back(this->allTris.size());
        m->load();
        for(Triangle* t :  m->getTris()){
            this->allTris.push_back(t);
            this->toModel.push_back(modelIndex);
        }
        modelIndex++;
    }
}

std::vector<Triangle *> Scene::getTris() {
    if(allTris.empty()) throw std::runtime_error("Scene::getTris(): load must be called prior, you have no triangles");
    return this->allTris;
}

ModelLoader* Scene::getModel(int modelIndex) {
    return this->models[modelIndex];
}

int Scene::getModelFromTri(int triIndex) {
    return this->toModel[triIndex];
}

int Scene::getModelOffset(int modelIndex) {
    return this->modelOffset[modelIndex];
}

glm::vec3 *Scene::getModelPosition(int modelIndex) {
    return this->models[modelIndex]->getPos();
}

void Scene::setModelPosition(int modelIndex, glm::vec3 pos) {
    this->models[modelIndex]->setPos(pos);
}

std::vector<glm::vec4 *> Scene::getLights() {
    return this->lights;
}

std::vector<glm::vec3> Scene::getLightLocs() {
    return this->lightLocs;
}

std::vector<float *> Scene::getLightStrengths() {
    return this->lightStrengths;
}

std::vector<float> Scene::getInitBrightnesses() {
    return this->initBrightnesses;
}

std::vector<float> Scene::getInitSpeculars(){
    return this->initSpeculars;
}

int Scene::getNumLights() {
    return this->numLights;
}
