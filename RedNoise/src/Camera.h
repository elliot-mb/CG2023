//
// Created by elliot on 16/10/23.
//

#pragma once


#include "glm/glm.hpp"
#include "DepthBuffer.h"

class Camera {
public:
    Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen);

    glm::vec3 getCanvasIntersectionPoint(glm::vec3 vertexPosition);
    void move(glm::vec3 delta); //move
    void setPos(glm::vec3 pos); //set
    glm::vec3 getPos();
    void rot(float angleX, float angleY); //move
    void setRot(float angleX, float angleY); //set
private:

    glm::mat3 orientation;
    glm::vec3 position;
    float focalLength;
    glm::vec2 screen;
};