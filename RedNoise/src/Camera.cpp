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
    this->screen2 = screen * static_cast<float>(0.5); //just do this once
    this->orientation = glm::mat3({1, 0, 0},
                           {0, 1, 0},
                           {0, 0, 1});
    this->isOrbiting = false;
    this->mode = msh;
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
    float u = (static_cast<float>((this->focalLength * ((-dRot.x) / dRot.z) * this->screen2.x) + (this->screen2.x)));
    float v = (static_cast<float>((this->focalLength * (dRot.y / dRot.z) * this->screen2.x) + (this->screen2.y)));
    float dist = 1 / dRot.z; //NOT 1 / glm::length(dRot); //z is literally the depth from the camera
    return std::tuple<glm::vec3, bool>{glm::vec3(u, v, dist), true}; //dist is the distance to the camera squared
}

//please given -1 for fobiddenIndex if there is no index forbade
pair<bool, pair<int, glm::vec3>> Camera::getClosestIntersection(int forbiddenIndex, glm::vec3 origin, glm::vec3 rayDir, ModelLoader& model){
    glm::vec3 closestValid = glm::vec3(0, 0, 0); //compare on t (x)
    int closestTri = 0;
    int count = 0;
    bool valid = false;

    if(model.getTris().empty())
        throw runtime_error("Camera::getClosestIntersection: this model has no tris");
    for(Triangle& triangle : model.getTris()){
        if(count != forbiddenIndex){
            glm::vec3 e0 = triangle.v1() - triangle.v0();
            glm::vec3 e1 = triangle.v2() - triangle.v0();
            glm::vec3 spVector = origin - triangle.v0(); //origin generalises so we can compute shadows
            glm::mat3 diff(-rayDir, e0, e1);
            glm::vec3 possibleSolution = glm::inverse(diff) * spVector;
            float* t = &possibleSolution.x;
            float* u = &possibleSolution.y;
            float* v = &possibleSolution.z;
            float tSq = *t * *t;
            float closeSq = closestValid.x * closestValid.x;
            if(*t > 0 && //is the ray colliding in front of the point (along the ray line)
               *u <= 1 && *u >= 0 &&
               *v <= 1 && *v >= 0 &&
               *u + *v <= 1 &&
               (tSq < closeSq || !valid)){ //if it hits the triangle AND its the new smallest OR the first we've seen
                valid = true;
                closestValid = possibleSolution;
                closestTri = count;
            }
        }
        count++;
    }
    return std::pair<bool, std::pair<int, glm::vec3>>{valid, {closestTri, closestValid}};
}
//
glm::vec3 Camera::buildCameraRay(int x, int y){
    glm::vec2 imagePlanePos = (glm::vec2(x, y) - glm::vec2(this->screen2.x, this->screen2.y)) / this->screen2.x; //inverse of what we did for rasterising
    glm::vec3 infront = this->focalLength * (glm::mat3(-1) * myFwd()); //where does the image plane origin start (flipping is required because of the discrepency between myFwd and the global fwd)
    glm::vec3 ray = infront + (imagePlanePos.x * myRight()) - (imagePlanePos.y * myUp());//relative to cam
    return ray; //unit length
}

void Camera::raycast(DrawingWindow& window, ModelLoader& model, glm::vec3 lightSource){

    std::vector<Triangle> tris = model.getTris();
    for(int x = 0; x < static_cast<int>(glm::floor(this->screen.x)); x++){
        for(int y = 0; y < static_cast<int>(glm::floor(this->screen.y)); y++){
            glm::vec3 ray = buildCameraRay(x, y);
            //first, cast from the camera to the scene
            MaybeTriangle intersection = getClosestIntersection(-1, this->position, ray, model);
            bool hasIntersection = intersection.first; //unpack pairs
            int triangleIndex = intersection.second.first;
            float distAlongRay = intersection.second.second.x;
            if(hasIntersection){ //if its valid...
                //...cast from the intersection to the light if one is given
                glm::vec3 intercept = this->position + (distAlongRay * ray); //tried with ray, lets also try with a tri
                glm::vec3 shadowRay = lightSource - intercept;
                MaybeTriangle shadowRayIntscnt = getClosestIntersection(triangleIndex, intercept, shadowRay, model);
                float distAlongShadowRay = shadowRayIntscnt.second.second.x;
                bool inShadow = shadowRayIntscnt.first && (distAlongShadowRay < 1); //less than 1 means we have not hit a triangle behind the light
                Colour c = tris[triangleIndex].getColour(); //find out what colour we draw it
                if(inShadow) c = Colour(0, 0, 0);
                window.setPixelColour(x, y, Utils::pack(255, c.red, c.green, c.blue));
            }
        }
    }
}

void Camera::rasterise(DrawingWindow& window, ModelLoader& model, DepthBuffer& depthBuffer){
    depthBuffer.reset();
    vector<Triangle> tris = model.getTris();
    for(size_t i = 0; i < tris.size(); i++){
        Triangle thisTri = tris[tris.size() - i - 1]; //tested to see if rendering them in reverse order has any effect
        auto [pt0, valid0] = getCanvasIntersectionPoint(thisTri.v0()); //project to flat (z becomes the distance to the camera)
        auto [pt1, valid1] = getCanvasIntersectionPoint(thisTri.v1());
        auto [pt2, valid2] = getCanvasIntersectionPoint(thisTri.v2());
        if(valid0 && valid1 && valid2){
            Colour thisColour = thisTri.getColour();
            thisTri.setV0(pt0);
            thisTri.setV1(pt1);
            thisTri.setV2(pt2);
            if(this->mode == msh){
                thisTri.draw(window);
            }
            if(thisTri.isTextured() && this->mode == rst) {
                thisTri.fillTexture(window, depthBuffer);
            }
            if(!thisTri.isTextured() && this->mode == rst) {
                thisTri.fill(window, depthBuffer);
            }
        }
    }
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

void Camera::renderMode() {
    this->mode = (this->mode + 1) % 3;
}

void Camera::doRaytracing(DrawingWindow& window, ModelLoader& model, glm::vec3 lightSource) {
    if(this->mode == ray){
        raycast(window, model, lightSource);
    }
}

void Camera::doRasterising(DrawingWindow &window, ModelLoader &model, DepthBuffer &depthBuffer){
    if(this->mode == rst || this->mode == msh){
        rasterise(window, model, depthBuffer);
    }
}
