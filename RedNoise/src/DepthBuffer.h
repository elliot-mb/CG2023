//
// Created by elliot on 22/10/23.
//

#include <vector>
#include "glm/glm.hpp"

#pragma once

class DepthBuffer {
public:
    DepthBuffer(int width, int height);
    // place a pixel in the buffer in a position with a value. returns boolean whether it is now the pixel in that position or not
    // based on epthBuffer* db = new DepthBuffer(WIDTH, HEIGHT);the provided z depth (encoding the distance to the camera)
    bool putPixel(glm::vec3 pos);
    void reset(); //sets all buff positions to zero
    void show(int strideX, int strideY); //print the depth buffer or every stride element in every stride column
private:
    int width;
    int height;
    std::vector<std::vector<float>> buff; //no direct access to the buffer
};

