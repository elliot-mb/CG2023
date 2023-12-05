//
// Created by elliot on 29/11/23.
//

#include "Light.h"
#include "Utils.h"

Light::Light(glm::mat3 light, glm::vec3 colour, float strength, int points, int segments) {
    this->pos = light[0];
    this->u = light[1];
    this->v = light[2];
    this->colour = colour;
    this->strength = strength / static_cast<float>(points * segments * segments);
    this->pts = {};
    glm::vec3 uStep = u / static_cast<float>(segments);
    glm::vec3 vStep = v / static_cast<float>(segments);
    for(int i = 0; i < segments; i++){ //generates randoms in a grid of cells
        glm::vec3 uStart = static_cast<float>(i) * uStep;
        for(int j = 0; j < segments; j++){
            glm::vec3 vStart = static_cast<float>(j) * vStep;
            for(int k = 0; k < points; k++){
                pts.push_back(this->pos + uStart + vStart + (Utils::getRandom() * uStep) + (Utils::getRandom() * vStep));
            }

        }
    }
    Colour c = Colour(0, 0, 0);
    this->surface1 = Triangle(pos, pos + u, pos + v, c);
    this->surface2 = Triangle(pos + u, pos + v, pos + u + v, c);
}

glm::vec3& Light::getColour(){
    return this->colour;
}

std::vector<glm::vec3> Light::getPts() const{
    return this->pts;
}

float Light::getStrength() const{ return this->strength; }

Triangle &Light::getSurface1() {
    return this->surface1;
}

Triangle &Light::getSurface2() {
    return this->surface2;
}



