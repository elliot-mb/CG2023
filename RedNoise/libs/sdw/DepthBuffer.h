//
// Created by elliot on 22/10/23.
//

#include <vector>
#include <glm/glm.hpp>

#pragma once

class DepthBuffer {
public:
    DepthBuffer(int width, int height);
    // place a pixel in the buffer in a position with a value. returns boolean whether it is now the pixel in that position or not
    // based on the provided z depth (encoding the distance to the camera)
    bool putPixel(glm::vec3 pos);
private:
    std::vector<std::vector<float>> buff; //no direct access to the buffer
};

