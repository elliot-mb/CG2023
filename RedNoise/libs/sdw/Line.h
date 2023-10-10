#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "DrawingWindow.h"
#include "Colour.h"
#include <CanvasPoint.h>

/**
 * Concrete class Line
 * 'static' class for line methods
*/
namespace Line {
    std::vector<glm::vec2> pixels(glm::vec2 posA, glm::vec2 posB); //gives all the pixels we must draw
    //std::vector<glm::vec2> points(glm::vec2 posA, glm::vec2 posB); //just gives points on each step
    void draw(DrawingWindow& window, glm::vec2 posA, glm::vec2 posB, Colour &colour, float weight);
};