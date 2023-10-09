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
    Line(glm::vec2 a, glm::vec2 b);
    ~Line(); //any memory that a class allocates and owns it must destroy in it's
    //destructor

    //render the line with a specified colour and weight
    const void draw(glm::vec3 colour, float weight);

  private: 
    glm::vec2 a;
    glm::vec2 b;
};