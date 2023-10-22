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
glm::vec3 Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
    //assume vertex has already been transposed to world origin
    //y
    float dz = this->position.z - vertexPosition.z;
    float dy = this->position.y - vertexPosition.y;
    float dx = this->position.x - vertexPosition.x;
    float w2 = screen.x * 0.5;
    float h2 = screen.y * 0.5;
    float u = (static_cast<float>((this->focalLength * ((-dx) / dz) * w2) + (w2)));
    float v = (static_cast<float>((this->focalLength * (dy / dz) * w2) + (h2)));
    float dist = glm::sqrt((dx * dx) + (dy * dy) + (dz * dz)); //avoid doing sqrt for now as it an expensive operation
    return glm::vec3(u, v, dist); //dist is the distance to the camera squared
}

void Camera::move(glm::vec3 delta){
    this->position = this->position + delta;
}


