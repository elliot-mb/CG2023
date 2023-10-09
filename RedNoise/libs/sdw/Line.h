#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

/**
 * Concrete class Line
 * 'static' class for line methods
*/
namespace Line {
  void draw(glm::vec2 posA, glm::vec2 posB, glm::vec3 colour, float weight);
};