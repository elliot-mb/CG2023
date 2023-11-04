//
// Created by elliot on 16/10/23.
//

#pragma once


#include "glm/glm.hpp"
#include "DepthBuffer.h"
#include "ModelLoader.h"

class Camera {
public:
    Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen);

    std::tuple<glm::vec3, bool> getCanvasIntersectionPoint(glm::vec3 vertexPosition); //vertex and whether we should draw it (not off image plane)
    void move(glm::vec3 delta); //move
    void setPos(glm::vec3 pos); //set
    glm::vec3 getPos();
    void rot(float angleX, float angleY); //move
    void lookAt(glm::vec3 at); //set
    void moveRelative(glm::vec3 delta);
    void toggleOrbit();
    void doOrbit(ModelLoader model);
private:

    glm::vec3 myUp();
    glm::mat3 orientation;
    glm::vec3 position;
    float focalLength;
    glm::vec2 screen;
    glm::vec3 myFwd();

    bool isOrbiting;
};
