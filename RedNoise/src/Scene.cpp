//
// Created by elliot on 21/11/23.
//

#include "Scene.h"

#include <utility>

Scene::Scene(std::vector<ModelLoader *> models) {
    this->models = std::move(models);
    this->allTris = {};
}

void Scene::load() {
    this->allTris = {};
    for(ModelLoader* m : this->models){
        m->load();
        for(Triangle* t :  m->getTris()){
            this->allTris.push_back(t);
        }
    }
}

std::vector<Triangle *> Scene::getTris() {
    if(allTris.empty()) throw std::runtime_error("Scene::getTris(): load must be called prior, you have no triangles");
    return this->allTris;
}

ModelLoader& Scene::getModel(int num) {
    return *this->models[num];
}
