#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

namespace Utils{
  std::vector<std::string> split(const std::string &line, char delimiter);
  std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);
  std::vector<vec3> interpolateThreeElementValues(vec3 from, vec3 to, int numberOfValues);
  uint32_t pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
}
