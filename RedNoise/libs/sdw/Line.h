#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

/**
 * Concrete class Line
 * Properties and drawing methods that a line needs to 
 * describe and display itself. 
*/
class Line {
  
  public: 
    //render the line with a specified colour and weight
    const void draw(glm::vec2 posA, glm::vec2 posB, glm::vec3 colour, float weight);
};