
#pragma once

#include <vector>
#include "glm/glm.hpp"

class Light {
public:
    //mat3 is p, u and v of the plane, points is how many points we sample
    Light(glm::mat3 light, glm::vec3 colour, float strength, int points);

    glm::vec3& getColour();

private:
    glm::vec3 pos;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 colour;
    float strength;
    std::vector<glm::vec3> pts; //points on the light we sample
    std::vector<glm::vec3> &getPts();
};


