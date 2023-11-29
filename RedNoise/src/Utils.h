#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "Colour.h"


namespace Utils{
    float min(float a, float b);
    float max(float a, float b);
    std::vector<std::string> split(const std::string &line, char delimiter);
    std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);
    std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues);
    uint32_t pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
    Colour unpack(uint32_t);
    std::string fileAsString(std::string& filename);
    std::vector<glm::vec2> interpolateTwoElementValues(glm::vec2 from, glm::vec2 to, int steps);
    glm::mat3 pitch(float theta);
    glm::mat3 yaw(float theta);
    glm::mat3 rotateMeTo(glm::vec3 direction, glm::vec3 myUp = glm::vec3(0,1,0));
    glm::mat3 rod(glm::vec3 w, float theta);
    glm::vec3 asVec3(Colour& c);
    //glm::vec3 weightedMeanVects(glm::vec3& vects, std::vector<float> weights);
    glm::vec3 getRandomUnitVec3();

    float getRandom();
}
