#include <algorithm>
#include <sstream>
#include "Line.h"
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>
#include "DrawingWindow.h"

using namespace std;
using namespace glm;

void Line::draw(DrawingWindow& window, vec2 posA, vec2 posB, vec3 colour, float weight){
  vec2 delta = posB - posA;
  //for a line we have to draw (at least) the number of pixels that are along the
  //longest dimension
  /**
   * 
      @    @ @        @@
      @   @  @      @@
      5  @   5    @@
      @ @    @  @@
      @@     @@@
       @@5@@  @@@@10@@@@
  */
  float steps = Utils::max(abs(delta.x), abs(delta.y));
  vec2 stepSize = delta / steps;
  
  for(float i = 0.0; i < steps; i++){
    vec2 now = posA + (stepSize * i);
    //draws two orthogonal lines, one with length 1
    for(float j = 0.0; j < stepSize.x; j++){ //vertical line
      window.setPixelColour(round(now.x + j), round(now.y), Utils::pack(255, 255, 255, 255));
    }
    //starts at 1.0 because we have guarenteed to have already drawn k = 0
    for(float k = 1.0; k < stepSize.y; k++){ //horizontal line 
      window.setPixelColour(round(now.x), round(now.y + k), Utils::pack(255, 0, 0, 0));
    }
  
  }
}