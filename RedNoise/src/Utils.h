#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"


namespace Utils{
  float min(float a, float b);
  float max(float a, float b);
  std::vector<std::string> split(const std::string &line, char delimiter);
  std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);
  std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues);
  uint32_t pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
  std::string fileAsString(std::string& filename);
  std::vector<glm::vec2> interpolateTwoElementValues(glm::vec2 from, glm::vec2 to, int steps);
  glm::mat3 rotateX(float theta);
  glm::mat3 rotateY(float theta);
}