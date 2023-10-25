//
// Created by elliot on 16/10/23.
//

#include "Camera.h"
#include "math.h";
#include "Utils.h"

Camera::Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen) {
    this->position = cameraPosition;
    this->focalLength = focalLength;
    this->screen = screen;
    this->orientation = glm::mat3({1, 0, 0},
                           {0, 1, 0},
                           {0, 0, 1});
}

// seems to assume the camera can only point in the negative z direction
glm::vec3 Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
    //assume vertex has already been transposed to world origin
    //y
    float dz = this->position.z - vertexPosition.z;
    float dy = this->position.y - vertexPosition.y;
    float dx = this->position.x - vertexPosition.x;
    glm::vec3 dRot = this->orientation * (glm::vec3(dx, dy, dz));
    float w2 = screen.x * 0.5;
    float h2 = screen.y * 0.5;
    float u = (static_cast<float>((this->focalLength * ((-dRot.x) / dRot.z) * w2) + (w2)));
    float v = (static_cast<float>((this->focalLength * (dRot.y / dRot.z) * w2) + (h2)));
    float dist = glm::sqrt((dRot.x * dRot.x) + (dRot.y * dRot.y) + (dRot.z * dRot.z));
    return glm::vec3(u, v, dist); //dist is the distance to the camera squared
}

void Camera::move(glm::vec3 delta){
    this->position = this->position + delta;
}


void Camera::setPos(glm::vec3 pos) {
    this->position = pos;
}

void Camera::rot(float angleX, float angleY) {
    this->orientation = orientation * Utils::rotateX(angleX) * Utils::rotateY(angleY);
}

void Camera::setRot(float angleX, float angleY) {
    this->orientation = glm::mat3({1, 0, 0},
                                  {0, 1, 0},
                                  {0, 0, 1});
    rot(angleX, angleY);
}

glm::vec3 Camera::getPos() {
    return this->position;
}
