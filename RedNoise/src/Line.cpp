#include <algorithm>
#include "Line.h"
#include "Utils.h"
#include <vector>
#include "glm/glm.hpp"
#include "DrawingWindow.h"
#include "Colour.h"
#include "TextureMap.h"
#include "DepthBuffer.h"

void Line::draw(DrawingWindow& window, glm::vec2 posA, glm::vec2 posB, Colour &colour, float weight){
    std::vector<glm::vec2> pixels = Line::pixels(posA, posB);
    for(glm::vec2 & pixel : pixels){
        int x = static_cast<int>(round(pixel.x));
        int y = static_cast<int>(round(pixel.y));
        if(x > 0 && y > 0 && x < static_cast<int>(window.width) && y < static_cast<int>(window.height)) {
            window.setPixelColour(x, y,Utils::pack(255, colour.red, colour.green, colour.blue));
        }
    }
}

void Line::draw(DrawingWindow& window, DepthBuffer& db, glm::vec3 posA, glm::vec3 posB, glm::vec2 posTA, glm::vec2 posTB, TextureMap& texture, float weight){
    std::vector<glm::vec3> pixels = Line::pixels(posA, posB);
    std::vector<glm::vec2> pixelsT = Line::pixels(posTA, posTB);
    std::vector<float> depths = Utils::interpolateSingleFloats(posA.z, posB.z, static_cast<int>(pixels.size()));
    float textureStep = static_cast<float>(pixelsT.size()) / static_cast<float>(pixels.size());
    float texturePoint = 0; //start at 0th texture pixel
    int i = 0;
    for(glm::vec3 & pixel : pixels){
        if(pixel.x > 0 && pixel.y > 0 && pixel.x < static_cast<float>(window.width) && pixel.y < static_cast<float>(window.height)){
            int x = static_cast<int>(round(pixel.x));
            int y = static_cast<int>(round(pixel.y));

            float z = depths[i];
            uint32_t pt = texture.pixel(static_cast<int>(pixelsT[floor(texturePoint)].x) % texture.width, static_cast<int>(pixelsT[floor(texturePoint)].y) % texture.width);
            if(db.putPixel(glm::vec3(x, y, pixel.z))){
                window.setPixelColour(static_cast<ulong>(pixel.x), static_cast<ulong>(pixel.y), pt);
            }
            texturePoint += textureStep;
        }
        i++;
    }

}

// this function is used just with a 2d depth matrix that stores the colour and
void Line::draw(DrawingWindow& window, DepthBuffer& db, glm::vec3 posA, glm::vec3 posB, Colour &colour, float weight){
    std::vector<glm::vec3> pixels = Line::pixels(posA, posB);
    for(glm::vec3& pixel : pixels){
        if(pixel.x > 0 && pixel.y > 0 && pixel.x < static_cast<float>(window.width) && pixel.y < static_cast<float>(window.height)){
            int x = static_cast<int>(round(pixel.x));
            int y = static_cast<int>(round(pixel.y));
            if(db.putPixel(glm::vec3(x, y, pixel.z))){
                window.setPixelColour(static_cast<ulong>(x), static_cast<ulong>(y), Utils::pack(255, colour.red, colour.green, colour.blue));
            }
        }
    }
}

std::vector<glm::vec2> Line::pixels(glm::vec2 posA, glm::vec2 posB){
    glm::vec2 delta = posB - posA;
    float steps = Utils::max(abs(delta.x), abs(delta.y));
    if(steps == 0) steps = 1;
    glm::vec2 stepSize = delta / steps;
    std::vector<glm::vec2> pixels = {};
    glm::vec2 now;
    for(int i = 0; i <= static_cast<int>(floor(steps)); i++){
        now = posA + (stepSize * static_cast<float>(i));
        pixels.push_back(glm::vec2(round(now.x), round(now.y)));
    }
    pixels.push_back(glm::vec2(round(posB.x), round(posB.y)));
    return pixels;
}

//all of these vec3s are projected onto the image plane, and just retain their z from distance to the camera. z is interpolated for occlusion purposes
//stored in the z is the distance to the camera, which is reciprocated and compared in the draw function which takes an additional parameter of a depth buffer
std::vector<glm::vec3> Line::pixels(glm::vec3 posA, glm::vec3 posB){
    glm::vec3 delta = posB - posA;
    float steps = Utils::max(abs(delta.x), abs(delta.y)); //steps depends just on the largest distance we have to cross in the (2D) projected plane
    if(steps == 0) steps = 1;
    glm::vec3 stepSize = delta / steps;
    std::vector<glm::vec3> pixels = {};
    glm::vec3 now;

    for(int i = 0; i <= static_cast<int>(floor(steps)); i++){
        now = posA + (stepSize * static_cast<float>(i));
        pixels.push_back(glm::vec3(glm::round(now.x), glm::round(now.y), now.z));
    }
    pixels.push_back(round(posB));
    return pixels;
}
