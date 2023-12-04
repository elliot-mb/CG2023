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

    Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen, Scene* s, int threads);
    std::tuple<glm::vec3, bool> getCanvasIntersectionPoint(glm::vec3 vertexPosition); //vertex and whether we should draw it (not off image plane)
    void move(glm::vec3 delta); //move
    void setPos(glm::vec3 pos); //set
    glm::vec3 getPos();
    void rot(float angleToZ, float angleToX); //move
    void lookAt(glm::vec3 at); //set
    void moveRelative(glm::vec3 delta);
    void toggleOrbit();
    void renderMode();
    void setRenderMode(int m);
    void doOrbit(ModelLoader model);
    void orbit(ModelLoader model, float angle);
    static std::pair<int, float> getClosestIntersection(int& forbiddenIndex, glm::vec3& origin, glm::vec3& rayDir, std::vector<Triangle*>& tris, Scene& scene, glm::vec2& intersectLoc = DEFAULT_INTERSECT);
    void doRaytracing(DrawingWindow &window);
    void doRasterising(DrawingWindow &window, DepthBuffer &depthBuffer);
    void setRot(float angleToZ, float angleToX);
    vec2 getRot();
    enum Mode {msh, rst, ray};//one two or three (mesh, raster, raycast)
private:
    static int NO_INTERSECTION;
    static glm::vec2 DEFAULT_INTERSECT; //gives optionality to getClosestIntersection as to whether we care to return barycentrics
    static glm::vec3 LIGHT_COLOUR;
    static glm::vec3 BACKGROUND_COLOUR;

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
    int threads;
    std::vector<int> sliceHeights; //0, p, q, r, s gives you the slices 0 -> p, p+1 -> q, q+1 -> r, r+1 -> s, s+1 -> screen height

    bool isOrbiting;
    uint mode;

    vec3 myFwd();

    vec3 myRight();

    vec3 buildCameraRay(int& x, int& y);
//    void raycast(DrawingWindow &window);
    void hit(int bounces, glm::vec3& source, glm::vec3& incidentRay, glm::vec2& vw, std::pair<int, float>& intersection, std::vector<Triangle*>& tris, glm::vec3& colour, float lastRefractI)/* const*/;
    void rasterise(DrawingWindow &window, DepthBuffer &depthBuffer);

    //lighting effects
    void proximity(float& brightness, float& len, float& strength);
    void diffuse(float &brightness, vec3 &shadowRay, vec3 &norm);

    void shadow(float &brightness, vec3 &shadowRay, int& intersection, vec3 &intercept, vector<Triangle *> &tris);

    void specular(float &brightness, float strength, vec3 &shadowRay, vec3 &norm, vec3 &camRay, float power);

    void
    gouraud(float &brightness, float strength, float& spec, vec3 &shadowRayn, float &u, float &v, float &w, vector<glm::vec3 *> &norms,
            vec3 &camRay, float& len);

    void reflectCast(int bounces, glm::vec3& topColour, vec3 &incidentRay, float& attenuation, pair<int, float> &intersection, vec3 &intercept, vec3 &norm,
                 vector<Triangle *>& tris, vec3 &colour)/* const*/;

    void raycast(DrawingWindow &window, int start, int end);

    vec3 refract(vec3 &norm, vec3 &incident, float ri1, float ri2);

    vec3 reflect(vec3 &norm, vec3 &incident);

    bool willInternallyReflect(vec3 &norm, vec3 &incident, float ri1, float ri2);

    vec3 envColour(vec3 escapedRay);

    static vec3 MIRROR_COLOUR;
};
