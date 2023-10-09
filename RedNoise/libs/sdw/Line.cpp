#include <algorithm>
#include <sstream>
#include "Line.h"
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>
#include "DrawingWindow.h"
#include <Colour.h>
#include <CanvasPoint.h>

using namespace std;
using namespace glm;

void Line::draw(DrawingWindow& window, vec2 posA, vec2 posB, Colour &colour, float weight){
  vec2 delta = posB - posA;
  //bool flatter = abs(delta.x) > abs(delta.y);
  //vec2 signs = delta / abs(delta);
  float steps = Utils::max(abs(delta.x), abs(delta.y));
  if(steps == 0) steps = 1;
  vec2 stepSize = delta / steps;

  vec2 last = posA;

  for(float i = 0; i <= steps; i++){
    vec2 now = posA + (stepSize * i);
    //window.setPixelColour(round(now.x), round(now.y), Utils::pack(255, 255, 255, 255));

    vec2 lastDelta = round(now) - last;
    vec2 signs = abs(lastDelta) / lastDelta;
    // x
    for(float j = 0.0; j < abs(lastDelta.x); j++){
        window.setPixelColour(round(last.x + (j * signs.x)), round(last.y), Utils::pack(255, colour.red, colour.green, colour.blue));
    }
    // y
    for(float j = 0.0; j < abs(lastDelta.y); j++){
        window.setPixelColour(round(last.x), round(last.y + (j * signs.y)), Utils::pack(255, colour.red, colour.green, colour.blue));
    }
    last = round(now);
  }
  window.setPixelColour(round(posB.x), round(posB.y), Utils::pack(255, colour.red, colour.green, colour.blue));
}