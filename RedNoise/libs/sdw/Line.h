#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "DrawingWindow.h"

/**
 * Concrete class Line
 * 'static' class for line methods
*/
namespace Line {
  void draw(DrawingWindow& window, glm::vec2 posA, glm::vec2 posB, glm::vec3 colour, float weight);
};