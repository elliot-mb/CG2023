//
// Created by elliot on 16/10/23.
//

#pragma once


#include "glm/glm.hpp"
#include "DepthBuffer.h"
#include "ModelLoader.h"
#include "Scene.h"

using namespace std;
using namespace glm;

class Camera {
public:
    typedef std::pair<bool, pair<int, glm::vec3>> MaybeTriangle;

    Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen, Scene* s);
    std::tuple<glm::vec3, bool> getCanvasIntersectionPoint(glm::vec3 vertexPosition); //vertex and whether we should draw it (not off image plane)
    void move(glm::vec3 delta); //move
    void setPos(glm::vec3 pos); //set
    glm::vec3 getPos();
    void rot(float angleZ, float angleX); //move
    void lookAt(glm::vec3 at); //set
    void moveRelative(glm::vec3 delta);
    void toggleOrbit();
    void renderMode();
    void setRenderMode(int m);
    void doOrbit(ModelLoader model);
    static std::pair<int, float> getClosestIntersection(int& forbiddenIndex, glm::vec3& origin, glm::vec3& rayDir, std::vector<Triangle*>& tris, Scene& scene, glm::vec2& intersectLoc = DEFAULT_INTERSECT);
    void doRaytracing(DrawingWindow &window);
    void doRasterising(DrawingWindow &window, DepthBuffer &depthBuffer);
    void setRot(float angleX, float angleY);
    vec2 getRot();
    enum Mode {msh, rst, ray};//one two or three (mesh, raster, raycast)
private:
    static int NO_INTERSECTION;
    static glm::vec2 DEFAULT_INTERSECT; //gives optionality to getClosestIntersection as to whether we care to return barycentrics
    static glm::vec3 LIGHT_COLOUR;

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
    std::vector<vector<glm::vec2>> imageCoords; // precomputed and referenced in buildCameraRays
    float ambientUpper;
    float ambientLower;
    Scene* scene;

    bool isOrbiting;
    uint mode;

    vec3 myFwd();

    vec3 myRight();

    vec3 buildCameraRay(int& x, int& y);
    void raycast(DrawingWindow &window);
    void hit(int bounces, glm::vec3& source, glm::vec3& incidentRay, glm::vec2& vw, std::pair<int, float>& intersection, std::vector<Triangle*>& tris, glm::vec3& colour);
    void rasterise(DrawingWindow &window, DepthBuffer &depthBuffer);

    //lighting effects
    void proximity(float& brightness, float& len, float& strength);
    void diffuse(float &brightness, vec3 &shadowRay, vec3 &norm);

    void shadow(float &brightness, vec3 &shadowRay, int& intersection, vec3 &intercept, vector<Triangle *> &tris, Scene& scene);

    void specular(float &brightness, vec3 &shadowRay, vec3 &norm, vec3 &camRay, float power);

    void
    gouraud(float &brightness, float& spec, vec3 &shadowRayn, float &u, float &v, float &w, vector<glm::vec3 *> &norms,
            vec3 &camRay, float& len, float& strength);

    void reflect(int bounces, glm::vec3& attenuation, float& fuzz, vec3 &incidentRay, pair<int, float> &intersection, vec3 &intercept, vec3 &norm,
                 vector<Triangle *>& tris, vec3 &colour);
};
