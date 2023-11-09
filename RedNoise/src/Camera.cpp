//
// Created by elliot on 16/10/23.
//

#include "Camera.h"
#include "math.h";
#include "Utils.h"
#include "ModelLoader.h"
#include <iostream>
#include <tuple>

Camera::Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen) {
    this->position = cameraPosition;
    this->focalLength = focalLength;
    this->screen = screen;
    this->orientation = glm::mat3({1, 0, 0},
                           {0, 1, 0},
                           {0, 0, 1});
    this->isOrbiting = false;
    this->isRaytracing = false;
}

// seems to assume the camera can only point in the negative z direction
std::tuple<glm::vec3, bool> Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
    //assume vertex has already been transposed to world origin
    //y
    glm::vec3 dPos = this->position - vertexPosition;
    glm::vec3 dRot = this->orientation * dPos;

    if(dRot.z < this->focalLength){
        //behind image plane
        return std::tuple<glm::vec3, bool>{glm::vec3(0, 0, 0), false};
    }
    float w2 = this->screen.x * 0.5;
    float h2 = this->screen.y * 0.5;
    float u = (static_cast<float>((this->focalLength * ((-dRot.x) / dRot.z) * w2) + (w2)));
    float v = (static_cast<float>((this->focalLength * (dRot.y / dRot.z) * w2) + (h2)));
    float dist = 1 / dRot.z; //NOT 1 / glm::length(dRot); //z is literally the depth from the camera
    //std::cout << dist << std::endl;
    return std::tuple<glm::vec3, bool>{glm::vec3(u, v, dist), true}; //dist is the distance to the camera squared
}

std::pair<Triangle, bool> Camera::getClosestIntersection(glm::vec3 rayDir, ModelLoader& model){
    glm::vec3 closestValid = glm::vec3(0, 0, 0); //compare on t (x)
    int closestTri = 0;
    int count = 0;
    bool valid = false;

    if(model.getTris().empty())
        throw runtime_error("Camera::getClosestIntersection: this model has no tris");
    for(Triangle& triangle : model.getTris()){
        glm::vec3 e0 = triangle.v1() - triangle.v0();
        glm::vec3 e1 = triangle.v2() - triangle.v0();
        glm::vec3 spVector = this->position - triangle.v0();
        glm::mat3 diff(-rayDir, e0, e1);
        glm::vec3 possibleSolution = glm::inverse(diff) * spVector;
        float t = possibleSolution.x;
        float u = possibleSolution.y;
        float v = possibleSolution.z;
        float tSq = t * t;
        float closeSq = closestValid.x * closestValid.x;
        if(u <= 1 &&
           v <= 1 &&
           u >= 0 &&
           v >= 0 &&
           u + v <= 1 &&(tSq < closeSq || !valid)){ //if it hits the triangle AND its the new smallest OR the first we've seen
            valid = true;
            closestValid = possibleSolution;
            closestTri = count;
        }
        count++;
    }
    //std::cout << "has t " << closestValid.x << std::endl;

    return std::pair<Triangle, bool>{model.getTris()[closestTri], valid};
}
//
glm::vec3 Camera::buildCameraRay(int x, int y){
    float w2 = static_cast<float>(this->screen.x * 0.5);
    float h2 = static_cast<float>(this->screen.y * 0.5);
    glm::vec2 imagePlanePos = (glm::vec2(x, y) - glm::vec2(w2, h2)) / w2; //inverse of what we did for rasterising
    glm::vec3 infront = this->focalLength * myFwd(); //where does the image plane origin start
    glm::vec3 ray = infront - (imagePlanePos.x * myRight()) + (imagePlanePos.y * myUp());//relative to cam
    //std::cout << ray.x << "," << ray.y << "," << ray.z << std::endl;
    return ray; //unit length
}

void Camera::raycast(DrawingWindow& window, ModelLoader& model){
    for(int x = 0; x < static_cast<int>(glm::floor(this->screen.x)); x++){
        for(int y = 0; y < static_cast<int>(glm::floor(this->screen.y)); y++){
            glm::vec3 ray = buildCameraRay(x, y);
            std::pair<Triangle, bool> maybeTriangle = getClosestIntersection(ray, model);
            if(maybeTriangle.second){
                Colour c = maybeTriangle.first.getColour();
                window.setPixelColour(x, y, Utils::pack(255, c.red, c.green, c.blue));
            }else{
                //window.setPixelColour(x, y, Utils::pack(255, 0, 0, 0)); clear pixels does this anyway
            }
        }
    }
//    std::pair<Triangle, bool> triValid = getClosestIntersection(myFwd(), model);
//    if(!triValid.second){
//        std::cout << "ray hits nothing!" << std::endl;
//    }else{
//        std::cout << triValid.first.getColour() << std::endl;
//    }

}

void Camera::move(glm::vec3 delta){
    this->position = this->position + delta;
}

//moves the camera relative to its own coordinate system
void Camera::moveRelative(glm::vec3 delta){
    this->position += glm::transpose(this->orientation) * delta;

}

void Camera::setPos(glm::vec3 pos) {
    this->position = pos;
}

void Camera::rot(float angleX, float angleY) {
    this->orientation = Utils::pitch(angleX) * this->orientation; //does correct pitch rotation
    //rotate each camera axis around the world y respectively
    glm::mat3 yaw = Utils::yaw(angleY); //rotation matrix
    glm::vec3 camX = yaw * glm::vec3(this->orientation[0].x, this->orientation[1].x, this->orientation[2].x);
    glm::vec3 camY = yaw * glm::vec3(this->orientation[0].y, this->orientation[1].y, this->orientation[2].y);
    glm::vec3 camZ = yaw * glm::vec3(this->orientation[0].z, this->orientation[1].z, this->orientation[2].z);
    this->orientation[0] = {camX.x, camY.x, camZ.x};
    this->orientation[1] = {camX.y, camY.y, camZ.y};
    this->orientation[2] = {camX.z, camY.z, camZ.z};
}

void Camera::lookAt(glm::vec3 at) {
    glm::vec3 direction = this->position - at;
    this->orientation = Utils::rotateMeTo(direction, glm::vec3(0, 1, 0), true);
}
//
//void Camera::lookAt(glm::vec3 pt){
//    this->orientation[2] = this->position - pt;
//}

glm::vec3 Camera::getPos() {
    return this->position;
}

//normalised vector
glm::vec3 Camera::myUp() {
    return glm::normalize(glm::vec3(this->orientation[0].y,
                     this->orientation[1].y,
                     this->orientation[2].y));
}

glm::vec3 Camera::myFwd(){
    return glm::vec3(this->orientation[0].z,
                     this->orientation[1].z,
                     this->orientation[2].z);
}

glm::vec3 Camera::myRight(){
    return glm::vec3(this->orientation[0].x,
                     this->orientation[1].x,
                     this->orientation[2].x);
}


void Camera::toggleOrbit() {
    this->isOrbiting = !this->isOrbiting;
}

void Camera::doOrbit(ModelLoader model) {
    if(isOrbiting){
        glm::vec3 toModel = this->position - model.getPos();
        this->position = model.getPos() + (Utils::yaw(0.01) * toModel); //rotate then translate
        this->lookAt(model.getPos());

        //this->moveRelative(glm::vec3(0.1, 0.0, 0.0));
    }
}

void Camera::toggleRaytrace() {
    this->isRaytracing = !this->isRaytracing;
}

void Camera::doRaytracing(DrawingWindow& window, ModelLoader& model) {
    if(this->isRaytracing){
        raycast(window, model);
    }
}
