//
// Created by elliot on 22/10/23.
//

#include "DepthBuffer.h"
#include <glm/glm.hpp>

DepthBuffer::DepthBuffer(int width, int height) {
    for(int y = 0; y < height; y++){ //populate with zeros (1/infinity distance)
        std::vector<float> row = {};
        for(int x = 0; x < width; x++){
            row.push_back(0);
        }
        this->buff.push_back(row);
    }
}

//true if pixel is to be replaced
bool DepthBuffer::putPixel(glm::vec3 pos) {
    float recip = 1 / pos.z;
    int x = static_cast<int>(glm::floor(pos.x));
    int y = static_cast<int>(glm::floor(pos.y));
    if(recip > this->buff[y][x]){ //replace this pixel!
        buff[y][x] = recip;
    } //else do nothing and return false

    return recip > this->buff[y][x];
}