#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

namespace Utils{
  float min(float a, float b);
  float max(float a, float b);
  std::vector<std::string> split(const std::string &line, char delimiter);
  std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);
  std::vector<vec3> interpolateThreeElementValues(vec3 from, vec3 to, int numberOfValues);
  uint32_t pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
  string fileAsString(string& filename);
  vector<vec2> interpolateTwoElementValues(vec2 from, vec2 to, int steps);
}
