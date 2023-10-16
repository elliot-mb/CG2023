//
// Created by elliot on 16/10/23.
//

#include "Camera.h"

Camera::Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen) {
    this->position = cameraPosition;
    this->focalLength = focalLength;
    this->screen = screen;
}

// seems to assume the camera can only point in the negative z direction
glm::vec2 Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
    //assume vertex has already been transposed to world origin
    //y
    float dz = this->position.z - vertexPosition.z;
    float dy = this->position.y - vertexPosition.y;
    float dx = this->position.x - vertexPosition.x;
    float u = static_cast<float>(this->focalLength * (dx / dz) + (screen.x * 0.5));
    float v = static_cast<float>(this->focalLength * (dy / dz) + (screen.y * 0.5));
    return glm::vec2(u, v);
}