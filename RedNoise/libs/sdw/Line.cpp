#include <algorithm>
#include "Line.h"
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>
#include "DrawingWindow.h"
#include "Colour.h"
#include "TextureMap.h"
#include "DepthBuffer.h"

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

// this function is used just with a 2d depth matrix that stores the colour and
void Line::draw(DrawingWindow& window, DepthBuffer& db, glm::vec3 posA, glm::vec3 posB, Colour &colour, float weight){
    vector<vec3> pixels = Line::pixels(posA, posB);
    for(vec3& pixel : pixels){
        if(db.putPixel(pixel)){
            window.setPixelColour(static_cast<ulong>(round(pixel.x)), static_cast<ulong>(round(pixel.y)), Utils::pack(255, colour.red, colour.green, colour.blue));
        }
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

//all of these vec3s are projected onto the image plane, and just retain their z from distance to the camera. z is interpolated for occlusion purposes
//stored in the z is the distance to the camera, which is reciprocated and compared in the draw function which takes an additional parameter of a depth buffer
vector<vec3> Line::pixels(vec3 posA, vec3 posB){
    vec3 delta = posB - posA;
    float steps = Utils::max(abs(delta.x), abs(delta.y));
    if(steps == 0) steps = 1;
    vec3 stepSize = delta / steps;
    vector<vec3> pixels = {};
    for(int i = 0; i <= static_cast<int>(floor(steps)); i++){
        vec3 now = posA + (stepSize * static_cast<float>(i));
        pixels.push_back(round(now));
    }
    pixels.push_back(round(posB));
    return pixels;
}
