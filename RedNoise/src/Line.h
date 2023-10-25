#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "DrawingWindow.h"
#include "Colour.h"
#include "TextureMap.h"
#include "CanvasPoint.h"
#include "DepthBuffer.h"

/**
 * Concrete class Line
 * 'static' class for line methods
*/
namespace Line {
    std::vector<glm::vec2> pixels(glm::vec2 posA, glm::vec2 posB); //gives all the pixels we must draw
    std::vector<glm::vec3> pixels(glm::vec3 posA, glm::vec3 posB); //gives all the pixels we must draw
    //std::vector<glm::vec2> points(glm::vec2 posA, glm::vec2 posB); //just gives points on each step
    void draw(DrawingWindow& window, glm::vec2 posA, glm::vec2 posB, Colour &colour, float weight);
    void draw(DrawingWindow& window, DepthBuffer& db, glm::vec3 posA, glm::vec3 posB, Colour &colour, float weight);
    void draw(DrawingWindow& window, glm::vec2 posA, glm::vec2 posB, glm::vec2 posTA, glm::vec2 posTB, TextureMap& texture, float weight);
};