//
// Created by elliot on 16/10/23.
//

#pragma once


#include <glm/glm.hpp>

class Camera {
public:
    Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen);
    glm::vec2 getCanvasIntersectionPoint(glm::vec3 vertexPosition);
    void move(glm::vec3 delta);
private:
    glm::vec3 position;
    float focalLength;
    glm::vec2 screen;
};
