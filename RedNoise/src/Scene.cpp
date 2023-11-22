//
// Created by elliot on 21/11/23.
//

#include "Scene.h"

#include <utility>

Scene::Scene(std::vector<ModelLoader *> models, glm::vec3 lights) {
    this->models = std::move(models);
    this->allTris = {};
    this->toModel = {};
    this->modelOffset = {}; //the offset each model's triangles have in the allTris list
    this->light = light;
    load(); //private because we should really only do it once
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
