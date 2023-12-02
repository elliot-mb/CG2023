//
// Created by elliot on 29/11/23.
//

#include "Light.h"
#include "Utils.h"

Light::Light(glm::mat3 light, glm::vec3 colour, float strength, int points) {
    this->pos = light[0];
    this->u = light[1];
    this->v = light[2];
    this->colour = colour;
    this->strength = strength / static_cast<float>(points);
    this->pts = {};
    for(int i = 0; i < points; i++){
        pts.push_back(this->pos + (Utils::getRandom() * this->u) + (Utils::getRandom() * this->v));
    }
}

glm::vec3& Light::getColour(){
    return this->colour;
}

std::vector<glm::vec3> Light::getPts() const{
    return this->pts;
}

float Light::getStrength() const{ return this->strength; }



