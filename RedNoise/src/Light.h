
#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "Triangle.h"

class Light {
public:
    //mat3 is p, u and v of the plane, points is how many points we sample
    Light(glm::mat3 light, glm::vec3 colour, float strength, int points, int segments);

    glm::vec3& getColour();
    float getStrength() const;
    std::vector<glm::vec3> getPts() const;
    Triangle& getSurfaceTri1();
    Triangle& getSurfaceTri2();
private:
    glm::vec3 pos;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 colour;
    float strength;
    std::vector<glm::vec3> pts; //points on the light we sample
    Triangle surfaceTri1;
    Triangle surfaceTri2;
};


