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
    vector<vec2> pixels = Line::pixels(posA, posB);
    for(int i = 0; i <= pixels.size(); i++){
        window.setPixelColour(round(pixels[i].x), round(pixels[i].y), Utils::pack(255, colour.red, colour.green, colour.blue));
    }
}

vector<vec2> Line::pixels(vec2 posA, vec2 posB){
    vec2 delta = posB - posA;
    float steps = Utils::max(abs(delta.x), abs(delta.y));
    if(steps == 0) steps = 1;
    vec2 stepSize = delta / steps;
    vec2 last = posA;
    vector<vec2> pixels = {};
    for(float i = 0; i <= steps; i++){
        vec2 now = posA + (stepSize * i);
        vec2 lastDelta = round(now) - last;
        if(lastDelta.x == 0) lastDelta.x = 0.1;
        if(lastDelta.y == 0) lastDelta.y = 0.1;
        vec2 signs = abs(lastDelta) / lastDelta;
        // x
        for(float j = 0.0; j < abs(lastDelta.x); j++){
           pixels.push_back(vec2(round(last.x + (j * signs.x)), round(last.y)));
        }
        // y
        for(float j = 0.0; j < abs(lastDelta.y); j++){
            pixels.push_back(vec2(round(last.x), round(last.y + (j * signs.y))));
        }
        last = round(now);
    }
    pixels.push_back(vec2(round(posB.x), round(posB.y)));
    return pixels;
}

vector<vec2> Line::points(vec2 posA, vec2 posB){
    vec2 delta = posB - posA;
    float steps = Utils::max(abs(delta.x), abs(delta.y));
    if(steps == 0) steps = 1;
    vec2 stepSize = delta / steps;
    vec2 last = posA;
    vector<vec2> points = {}; //the dots along the line such that there is one for each y level
    for(float i = 0; i <= steps; i++){
        vec2 now = posA + (stepSize * i);
        points.push_back(vec2(round(now.x), round(now.y)));
    }
    points.push_back(vec2(round(posB.x), round(posB.y)));
    return points;
}