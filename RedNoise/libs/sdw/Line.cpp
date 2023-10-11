#include <algorithm>
#include <sstream>
#include "Line.h"
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>
#include "DrawingWindow.h"
#include "Colour.h"
#include "CanvasPoint.h"
#include "TextureMap.h"

using namespace std;
using namespace glm;

void Line::draw(DrawingWindow& window, vec2 posA, vec2 posB, Colour &colour, float weight){
    vector<vec2> pixels = Line::pixels(posA, posB);
    for(vec2 & pixel : pixels){
        window.setPixelColour(static_cast<ulong>(round(pixel.x)), static_cast<ulong>(round(pixel.y)), Utils::pack(255, colour.red, colour.green, colour.blue));
    }
}

void Line::draw(DrawingWindow& window, vec2 posA, vec2 posB, vec2 posTA, vec2 posTB, TextureMap& texture, float weight){
    vector<vec2> pixels = Line::pixels(posA, posB);
    vector<vec2> pixelsT = Line::pixels(posTA, posTB);
    float textureStep = static_cast<float>(pixelsT.size()) / static_cast<float>(pixels.size());
    float texturePoint = 0; //start at 0th texture pixel
    for(vec2 & pixel : pixels){
        uint32_t pt = texture.pixel(static_cast<int>(pixelsT[floor(texturePoint)].x), static_cast<int>(pixelsT[floor(texturePoint)].y));
        window.setPixelColour(static_cast<ulong>(round(pixel.x)), static_cast<ulong>(round(pixel.y)), pt);
        texturePoint += textureStep;
    }

}

vector<vec2> Line::pixels(vec2 posA, vec2 posB){
    vec2 delta = posB - posA;
    float steps = Utils::max(abs(delta.x), abs(delta.y));
    if(steps == 0) steps = 1;
    vec2 stepSize = delta / steps;
    vector<vec2> pixels = {};
    for(int i = 0; i <= static_cast<int>(floor(steps)); i++){
        vec2 now = posA + (stepSize * static_cast<float>(i));
        pixels.push_back(vec2(round(now.x), round(now.y)));
    }
    pixels.push_back(vec2(round(posB.x), round(posB.y)));
    return pixels;
}
