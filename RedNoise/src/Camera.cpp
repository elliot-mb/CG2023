//
// Created by elliot on 16/10/23.
//

#include "Camera.h"
#include "math.h";

Camera::Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen) {
    this->position = cameraPosition;
    this->focalLength = focalLength;
    this->screen = screen;
    this->rotX = glm::mat3({1, 0, 0},
                           {0, 1, 0},
                           {0, 0, 1});
    this->rotY = glm::mat3({1, 0, 0},
                           {0, 1, 0},
                           {0, 0, 1});
    this->xT = 0;
    this->yT = 0;
}

// seems to assume the camera can only point in the negative z direction
glm::vec3 Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
    //assume vertex has already been transposed to world origin
    //y
    float dz = this->position.z - vertexPosition.z;
    float dy = this->position.y - vertexPosition.y;
    float dx = this->position.x - vertexPosition.x;
    glm::vec3 dRot = (glm::vec3(dx, dy, dz) * this->rotX) * this->rotY;
    float w2 = screen.x * 0.5;
    float h2 = screen.y * 0.5;
    float u = (static_cast<float>((this->focalLength * ((-dRot.x) / dRot.z) * w2) + (w2)));
    float v = (static_cast<float>((this->focalLength * (dRot.y / dRot.z) * w2) + (h2)));
    float dist = (dRot.x * dRot.x) + (dRot.y * dRot.y) + (dRot.z * dRot.z); //avoid doing sqrt for now as it an expensive operation
    return glm::vec3(u, v, dist); //dist is the distance to the camera squared
}

void Camera::move(glm::vec3 delta){
    this->position = this->position + delta;
}


void Camera::setPos(glm::vec3 pos) {
    this->position = pos;
}

void Camera::makeRots() {
    if(xT < 0) xT += M_PI_2f * 4;
    if(yT < 0) yT += M_PI_2f * 4;
    float cTx = glm::cos(xT + (M_PI_2f * 4));
    float sTx = glm::sin(xT + (M_PI_2f * 4));
    float cTy = glm::cos(yT + (M_PI_2f * 4));
    float sTy = glm::sin(yT + (M_PI_2f * 4));
    this->rotX = glm::mat3({1,   0,    0},
                           {0, cTx, -sTx},
                           {0, sTx,  cTx});
    this->rotY = glm::mat3({cTy,  0, sTy},
                           {0,    1,   0},
                           {-sTy, 0, cTy});
}

void Camera::rot(float angleX, float angleY) {
    this->xT += angleX;
    this->yT += angleY;
    makeRots();
}

void Camera::setRot(float angleX, float angleY) {
    this->xT = angleX;
    this->yT = angleY;
    makeRots();
}

glm::vec3 Camera::getPos() {
    return this->position;
}
