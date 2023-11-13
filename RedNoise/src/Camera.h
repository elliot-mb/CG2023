//
// Created by elliot on 16/10/23.
//

#pragma once


#include "glm/glm.hpp"
#include "DepthBuffer.h"
#include "ModelLoader.h"

class Camera {
public:
    typedef std::pair<bool, pair<int, glm::vec3>> MaybeTriangle;

    Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen);

    std::tuple<glm::vec3, bool> getCanvasIntersectionPoint(glm::vec3 vertexPosition); //vertex and whether we should draw it (not off image plane)
    void move(glm::vec3 delta); //move
    void setPos(glm::vec3 pos); //set
    glm::vec3 getPos();
    void rot(float angleX, float angleY); //move
    void lookAt(glm::vec3 at); //set
    void moveRelative(glm::vec3 delta);
    void toggleOrbit();
    void renderMode();
    void doOrbit(ModelLoader model);
    std::pair<int, float> getClosestIntersection(int forbiddenIndex, glm::vec3 origin, glm::vec3 rayDir, ModelLoader &model);
    void doRaytracing(DrawingWindow &window, ModelLoader &model, glm::vec3 lightSource);
    void doRasterising(DrawingWindow &window, ModelLoader &model, DepthBuffer &depthBuffer);
private:

    glm::vec3 myUp(); //return normalised up vector
    glm::mat3 orientation; //the basis of the camera stored in columns [xxx^T, yyy^T, zzz^T]
    // a lot like
    /*
     * glm::mat3( {camX.x, camY.x, camZ.x},
                  {camX.y, camY.y, camZ.y},
                  {camX.z, camY.z, camZ.z})
     */
    glm::vec3 position; // offset from world origin
    float focalLength; // distance of the clipping plane along the z axis
    glm::vec2 screen; //resolution
    glm::vec2 screen2; //screen over 2

    bool isOrbiting;
    uint mode;
    enum Mode {msh, rst, ray};//one two or three (mesh, raster, raycast)
    vec3 myFwd();

    vec3 myRight();

    vec3 buildCameraRay(int x, int y);
    void raycast(DrawingWindow &window, ModelLoader &model, glm::vec3 lightSource);
    void rasterise(DrawingWindow &window, ModelLoader &model, DepthBuffer &depthBuffer);

};
