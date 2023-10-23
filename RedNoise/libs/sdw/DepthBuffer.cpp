//
// Created by elliot on 22/10/23.
//

#include "DepthBuffer.h"
#include <glm/glm.hpp>
#include <iostream>

DepthBuffer::DepthBuffer(int width, int height) {
    this->width = width;
    this->height = height;
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
    int x = static_cast<int>(pos.x); //these must be rounded
    int y = static_cast<int>(pos.y);
    if(x > width - 1 || x < 0){
        return false; //outside draw window
    }
    if(y > height - 1 || y < 0){
        return false; //outside draw window
    }
    if(recip > this->buff[y][x]){ //replace this pixel!
        buff[y][x] = recip;
        return true;
    } //else do nothing and return false

    return false;
}

void DepthBuffer::reset(){
    for(int y = 0; y < this->height; y++){ //populate with zeros (1/infinity distance)
        for(int x = 0; x < this->width; x++){
            this->buff[y][x] = 0;
        }
    }
}

void DepthBuffer::show(int strideX, int strideY){
    for(int x = 0; x < this->width; x+=strideX){
        std::cout << "--" << ' ';
    }
    std::cout << std::endl;
    for(int y = 0; y < this->height; y+=strideY){ //populate with zeros (1/infinity distance)
        for(int x = 0; x < this->width; x+=strideX){
            int readableOut = static_cast<int>( glm::round(this->buff[y][x] * 100));
            if(readableOut < 10) std::cout << '0';
            std::cout << readableOut << ' ';
        }
        std::cout << std::endl;
    }
}