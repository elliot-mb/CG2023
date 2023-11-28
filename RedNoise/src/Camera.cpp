//
// Created by elliot on 16/10/23.
//

#include "Camera.h"
#include "Utils.h"
#include "ModelLoader.h"
#include "Line.h"
#include "Scene.h"
#include <iostream>
#include <tuple>
#include <thread>

glm::vec2 Camera::DEFAULT_INTERSECT = {0.0, 0.0}; //gives reference to optional argument in getClosestIntersect
int Camera::NO_INTERSECTION = -1;
glm::vec3 Camera::LIGHT_COLOUR = glm::vec3({255, 255, 255});
glm::vec3 Camera::BACKGROUND_COLOUR = glm::vec3({0, 0, 0});

Camera::Camera(glm::vec3 cameraPosition, float focalLength, glm::vec2 screen, Scene* scene, int threads) {
    this->position = cameraPosition;
    this->focalLength = focalLength;
    this->screen = screen;
    this->screen2 = screen * static_cast<float>(0.5); //just do this once
    this->orientation = glm::mat3({1, 0, 0},
                           {0, 1, 0},
                           {0, 0, 1});
    this->isOrbiting = false;
    this->mode = ray;

    for(int y = 0; y < static_cast<int>(glm::floor(this->screen.y)); y++){
        imageCoords.push_back({});
        for(int x = 0; x < static_cast<int>(glm::floor(this->screen.x)); x++)
            this->imageCoords[y].push_back((glm::vec2(x, y) - glm::vec2(this->screen2.x, this->screen2.y)) / this->screen2.x); //inverse of what we did for rasterising
    }
         //populate
    this->ambientUpper = 0.95;
    this->ambientLower = 0.2;
    this->scene = scene;
    this->threads = threads;

    //make slice heights
    this->sliceHeights = {};
    int stride = static_cast<int>(screen.y) / this->threads;
    for(int i = 0; i < static_cast<int>(screen.y); i+=stride){ //we write a -1 in the first slice as we always take row value +1 as the start bound
        sliceHeights.push_back(i); //exclusivity will be guaranteed by the thread runner (doRaytracing)
    }
    sliceHeights.push_back(static_cast<int>(screen.y)); //the screenheight-1th row
}

// seems to assume the camera can only point in the negative z direction
std::tuple<glm::vec3, bool> Camera::getCanvasIntersectionPoint(glm::vec3 vertexPosition) {
    //assume vertex has already been transposed to world origin
    //y
    glm::vec3 dPos = this->position - vertexPosition;
    glm::vec3 dRot = this->orientation * dPos;
//
    if(dRot.z < 0){
        //behind image plane
        return std::tuple<glm::vec3, bool>{glm::vec3(0, 0, 0), false};
    }
    float u = (static_cast<float>((this->focalLength * ((-dRot.x) / dRot.z) * this->screen2.x) + (this->screen2.x)));
    float v = (static_cast<float>((this->focalLength * (dRot.y / dRot.z) * this->screen2.x) + (this->screen2.y)));
    float dist = 1 / dRot.z; //NOT 1 / glm::length(dRot); //z is literally the depth from the camera
    return std::tuple<glm::vec3, bool>{glm::vec3(u, v, dist), true}; //dist is the distance to the camera squared
}

//please given -1 for fobiddenIndex if there is no index forbade
//returns {index: int ::= index of triangle intersection OR -1 if not found, closestValid: float ::= distance along ray of collision}
//sets (by ref) the u and v of the intersection (intersectLoc) corresponding to the edges distance for conversion to barycentric coords
std::pair<int, float> Camera::getClosestIntersection(int& forbiddenIndex, glm::vec3& origin, glm::vec3& rayDir, std::vector<Triangle*>& tris, Scene& scene, glm::vec2& intersectLoc){
    float closestValid = INFINITY; //compare on t (x)
    int closestTri = -1;
    if(tris.empty())
        throw runtime_error("Camera::getClosestIntersection: this model has no tris");

    for(int i = 0; i < static_cast<int>(tris.size()); i++){
        if(i != forbiddenIndex){
            Triangle* tri = tris[i];
            glm::vec3 spVector = origin - tri->v0() - *(scene.getModel(scene.getModelFromTri(i))->getPos()); // final term is for the individual model position
            glm::mat3 diff(-rayDir, tri->e0, tri->e1);
            glm::vec3 possibleSolution = glm::inverse(diff) * spVector;
            if(possibleSolution.x > 0 && //is the ray colliding in front of the point (along the ray line)
               possibleSolution.y <= 1 && possibleSolution.y >= 0 &&
               possibleSolution.z <= 1 && possibleSolution.z >= 0 &&
               possibleSolution.y + possibleSolution.z <= 1 &&
               possibleSolution.x < closestValid){ //if it hits the triangle AND its the new smallest OR the first we've seen
                intersectLoc = {possibleSolution.y, possibleSolution.z}; //u and v to be returned via reference setting
                closestValid = possibleSolution.x;
                closestTri = i;
            }
        }
    }
    return {closestTri, closestValid}; //simply returns the index and distance along the ray
}
//
glm::vec3 Camera::buildCameraRay(int& x, int& y){
    glm::vec2 imagePlanePos = this->imageCoords[y][x]; //inverse of what we did for rasterising
    glm::vec3 infront = this->focalLength * (glm::mat3(-1) * myFwd()); //where does the image plane origin start (flipping is required because of the discrepency between myFwd and the global fwd)
    glm::vec3 ray = infront + (imagePlanePos.x * myRight()) - (imagePlanePos.y * myUp());//relative to cam
    return ray;
}

void Camera::proximity(float& brightness, float& len, float& strength){
    brightness = brightness * static_cast<float>(strength / glm::pow(len, 2));
}

void Camera::specular(float& brightness, glm::vec3& shadowRay, glm::vec3& norm, glm::vec3& camRay, float power){
    glm::vec3 incidentRay = glm::normalize(shadowRay - ((static_cast<float>(2.0) * norm) * (glm::dot(shadowRay, norm))));
    float similarity = glm::dot(glm::normalize(camRay), incidentRay);
    if(similarity < 0) return;
    float specStrength = static_cast<float>(glm::pow(similarity, power));
    brightness = static_cast<float>(brightness + specStrength);
}

void Camera::diffuse(float& brightness, glm::vec3& shadowRay, glm::vec3& norm){
    brightness = static_cast<float>(brightness * glm::dot(norm, shadowRay));
}

void Camera::shadow(float& brightness, glm::vec3& shadowRay, int& currentTri, glm::vec3& intercept,  std::vector<Triangle*>& tris, Scene& scene){
    std::pair<int, float> shadowRayIntscnt = getClosestIntersection(currentTri, intercept, shadowRay, tris, scene);
    if((shadowRayIntscnt.first != -1) && (shadowRayIntscnt.second < 1)) //less than 1 means we have not hit a triangle behind the light
        brightness = this->ambientLower;
}

void Camera::gouraud(float& brightness, float& spec, glm::vec3& shadowRayn, float& u, float& v, float& w, std::vector<glm::vec3 *>& norms, glm::vec3& camRay, float& len, float& strength){
    float diffV1 = 1.0; float diffV2 = 1.0; float diffV3 = 1.0; //characteristics for each vertex
    float specV1 = 0; float specV2 = 0; float specV3 = 0;
    specular(specV1, shadowRayn, *norms[0], camRay, 128);
    diffuse(diffV1, shadowRayn, *norms[0]);
    proximity(brightness, len, strength);
    specular(specV2, shadowRayn, *norms[1], camRay, 128);
    diffuse(diffV2, shadowRayn, *norms[1]);
    proximity(brightness, len, strength);
    specular(specV3, shadowRayn, *norms[2], camRay, 128);
    diffuse(diffV3, shadowRayn, *norms[2]);
    proximity(brightness, len, strength);
    //interpolate brightnesses
    brightness = brightness * static_cast<float>((diffV1 * u) + (diffV2 * v) + (diffV3 * w));
    spec = static_cast<float>((specV1 * u) + (specV2 * v) + (specV3 * w));
}

void Camera::reflect(int bounces, glm::vec3& attenuation, glm::vec3& incidentRay, std::pair<int, float>& intersection, glm::vec3& intercept, glm::vec3& norm, std::vector<Triangle*>& tris, vec3 &colour, glm::vec3& fuzz, int& x, int& y) /* const*/{
    glm::vec2 vwBounce;
    std::pair<int, float> reflectionIntersect;
    glm::vec3 incidentRayNrml = glm::normalize(incidentRay);
    glm::vec3 reflectedRay = glm::normalize(incidentRayNrml - (static_cast<float>(2) * norm * glm::dot(incidentRayNrml, norm)));
    reflectedRay = reflectedRay + fuzz; //add some random offset if present
    reflectionIntersect = getClosestIntersection(intersection.first, intercept, reflectedRay, tris, *this->scene,
                                                 vwBounce);
    if (reflectionIntersect.first != NO_INTERSECTION) {
        hit(bounces, intercept, reflectedRay, vwBounce, reflectionIntersect, tris, colour, x, y); //only recursive call
        colour = (colour * static_cast<float>(0.5)) + (attenuation * static_cast<float>(0.5)); //darkens it on the way back up the call stack in reverse hit order
        return;
    } else {
        colour = BACKGROUND_COLOUR * attenuation;
        return;
    }
}

//recursive raycast function for colouring surfaces, call itself again on reflection. It does not do the initial intersection
// old_body[incidentRay/camRay]
void Camera::hit(int bounces, glm::vec3 &source, glm::vec3& incidentRay, glm::vec2& vw, std::pair<int, float>& intersection, std::vector<Triangle*>& tris, vec3 &colour, int& x, int& y) /*const*/ {
    if(bounces < 0){
        colour = BACKGROUND_COLOUR;
        return;
    }
    bounces--; //decrement for each all to hit

    int modelIndex = scene->getModelFromTri(intersection.first);
    ModelLoader* model = scene->getModel(modelIndex);
    int modelTriIndex = intersection.first - scene->getModelOffset(modelIndex);
    Triangle* tri = tris[intersection.first];
    //...cast from the intersection to the light if one is given
    glm::vec3 intercept = source + (intersection.second * incidentRay);
    //glm::vec3 shadowRay = lightLoc - intercept;
    std::vector<glm::vec3> shadowRays = {};
    for(glm::vec3 loc : this->scene->getLightLocs()){
        shadowRays.push_back(loc - intercept);
    }
    //float len = glm::length(shadowRay);
    std::vector<float> lens = {};
    for(glm::vec3 shdR : shadowRays){
        lens.push_back(glm::length(shdR));
    }
//                glm::vec3 shadowRayn = glm::normalize(shadowRay);
    std::vector<glm::vec3> shadowRayNrmls = {};
    for(glm::vec3 shdR : shadowRays){
        shadowRayNrmls.push_back(glm::normalize(shdR));
    }
    //u v w in barycentric coordinates (wrt v0 being A, v1 being B, and v2 being C)
    float u = 1 - vw.x - vw.y;
    float v = vw.x;
    float w = vw.y;

    std::vector<float> brightnesses(this->scene->getInitBrightnesses()); //supposedly a deep copy
    std::vector<float> speculars(this->scene->getInitSpeculars());
    Colour c = tri->getColour(); //find out what colour we draw it (in most render methods thats the triangle colour)
    if(tri->isTextured()) c = tri->getTextureColour(u, v, w);

    glm::vec3 norm;
    std::vector<glm::vec3*> norms;
    int shading = *model->getShading();

    if(shading != ModelLoader::nrm){ //vertex normals are computed just if we need them
        norms = model->getNormsForTri(modelTriIndex); //needed for gouraud and phong
        norm = (*norms[0] * u) + (*norms[1] * v) + (*norms[2] * w); //needed just for phong
    }else{ //flat shading doesnt use vertex normals
        norm = *tri->getNormal();
    }

    if(shading == ModelLoader::mtl || shading == ModelLoader::phg_mtl){
        glm::vec3 tintedAttenuation = *model->getAttenuation() * Utils::asVec3(c);
        reflect(bounces, tintedAttenuation, incidentRay, intersection, intercept, norm, tris, colour, model->lookupFuzz(x, y), x, y);
        glm::vec3 fuzzNorm = norm + model->lookupFuzz(x, y);
        for (int i = 0; i < static_cast<int>(brightnesses.size()); i++) {
            specular(speculars[i], shadowRayNrmls[i], fuzzNorm, incidentRay, 16);
            //diffuse(brightnesses[i], shadowRayNrmls[i], norm);
        }
//        float finalBrightness = 0;
//        for(float brightness : brightnesses){ //total them up
//            finalBrightness += brightness;
//        }
        float finalSpecular = 0;
        for(float specular : speculars){
            finalSpecular += specular;
        }
//        if(finalBrightness > this->ambientUpper) finalBrightness = this->ambientUpper;
//        if(finalBrightness < this->ambientLower) finalBrightness = this->ambientLower; //if in shadow set to ambient
//        colour = finalBrightness * colour;
        if(finalSpecular > 1.0) finalSpecular = 1.0;
        colour = ((1 - finalSpecular) * colour) + (finalSpecular * LIGHT_COLOUR); //lerp on specular brightness between pixel colour and light colour
        return;
    }
    if(shading == ModelLoader::mrr || shading == ModelLoader::phg_mrr) { // if there is no intersection we return black (this will eventually be the skybox)
        glm::vec3 attenuation = *model->getAttenuation() * glm::vec3(1,1,1);
        reflect(bounces, attenuation, incidentRay, intersection, intercept, norm, tris, colour, ModelLoader::NO_FUZZ, x, y);
        return;
    }
    if(shading == ModelLoader::phg) {
        for (int i = 0; i < static_cast<int>(brightnesses.size()); i++) {
            specular(speculars[i], shadowRayNrmls[i], norm, incidentRay, 128);
            diffuse(brightnesses[i], shadowRayNrmls[i], norm);
            proximity(brightnesses[i], lens[i], *this->scene->getLightStrengths()[i]);
        }
    }
    if(shading == ModelLoader::grd){
        for(int i = 0; i < static_cast<int>(brightnesses.size()); i++) {
            gouraud(brightnesses[i], speculars[i], shadowRayNrmls[i], u, v, w, norms, incidentRay, lens[i], *this->scene->getLightStrengths()[i]);
        }
    }
    if(shading == ModelLoader::nrm) {
        for(int i = 0; i < static_cast<int>(brightnesses.size()); i++) {
            specular(speculars[i], shadowRayNrmls[i], norm, incidentRay, 128);
            diffuse(brightnesses[i], shadowRayNrmls[i], norm);
            shadow(brightnesses[i], shadowRays[i], intersection.first, intercept, tris, *this->scene);
            proximity(brightnesses[i], lens[i], *this->scene->getLightStrengths()[i]);
        }
    }

    float finalBrightness = 0;
    for(float brightness : brightnesses){ //total them up
        finalBrightness += brightness;
    }
    float finalSpecular = 0;
    for(float specular : speculars){
        finalSpecular += specular;
    }
    finalSpecular = finalSpecular / static_cast<float>(this->scene->getNumLights()); //normalise so we're between 0 and 1
    if(finalBrightness > this->ambientUpper) finalBrightness = this->ambientUpper;
    if(finalBrightness < this->ambientLower) finalBrightness = this->ambientLower; //if in shadow set to ambient

    colour = finalBrightness * Utils::asVec3(c);
    colour = ((1 - finalSpecular) * colour) + (finalSpecular * LIGHT_COLOUR); //lerp on specular brightness between pixel colour and light colour
}

//start and end row numbers of the strip
void Camera::raycast(DrawingWindow& window, int start, int end){
    std::vector<Triangle*> tris = scene->getTris();

    int stride = 1; //how large are our ray texturePts (1 is native resolution)
    int bounces = 2;


    for(int x = 0; x < static_cast<int>(glm::floor(this->screen.x)); x += stride){
        for(int y = start; y < end; y += stride){
            glm::vec3 camRay = buildCameraRay(x, y);
            //first, cast from the camera to the scene
            glm::vec2 vw = {0.0, 0.0};
            std::pair<int, float> intersection = getClosestIntersection(NO_INTERSECTION, this->position, camRay, tris, *scene, vw);
            if(intersection.first != NO_INTERSECTION){ //if its valid...
                glm::vec3 finalColour;
                ModelLoader* model = scene->getModel(scene->getModelFromTri(intersection.first));
//                if(*model->getFuzz() != 0)
//                    this->currentFuzz = model->lookupFuzz(x, y);
//                else this->currentFuzz = ModelLoader::NO_FUZZ;

                hit(bounces, this->position, camRay, vw, intersection, tris, finalColour, x, y);

                window.setPixelColour(x, y, Utils::pack(255,
                                                        static_cast<uint8_t>(finalColour.x),
                                                        static_cast<uint8_t>(finalColour.y),
                                                        static_cast<uint8_t>(finalColour.z)));
                if(stride > 1)
                    for(int i = 0; i < stride; i++){
                        for(int j = 0; j < stride; j++){
                            window.setPixelColour(x + i, y + j, Utils::pack(255,
                                                                            static_cast<uint8_t>(finalColour.x),
                                                                            static_cast<uint8_t>(finalColour.y),
                                                                            static_cast<uint8_t>(finalColour.z)));
                        }
                    }
            }else{
                window.setPixelColour(x, y, Utils::pack(255,
                                                                static_cast<uint8_t>(BACKGROUND_COLOUR.x),
                                                                static_cast<uint8_t>(BACKGROUND_COLOUR.y),
                                                                static_cast<uint8_t>(BACKGROUND_COLOUR.z)));
                if(stride > 1)
                    for(int i = 0; i < stride; i++){
                        for(int j = 0; j < stride; j++){
                            window.setPixelColour(x + i, y + j, Utils::pack(255,
                                                                            static_cast<uint8_t>(BACKGROUND_COLOUR.x),
                                                                            static_cast<uint8_t>(BACKGROUND_COLOUR.y),
                                                                            static_cast<uint8_t>(BACKGROUND_COLOUR.z)));
                        }
                    }
            }
        }
    }
}

void Camera::rasterise(DrawingWindow& window, DepthBuffer& depthBuffer){
    depthBuffer.reset();
    vector<Triangle*> tris = scene->getTris();
    for(size_t i = 0; i < tris.size(); i++){
        Triangle thisTri = *tris[i];
        ModelLoader* currentModel = scene->getModel(scene->getModelFromTri(static_cast<int>(i)));
        auto [pt0, valid0] = getCanvasIntersectionPoint(thisTri.v0() + *currentModel->getPos()); //project to flat (z becomes the distance to the camera)
        auto [pt1, valid1] = getCanvasIntersectionPoint(thisTri.v1() + *currentModel->getPos());
        auto [pt2, valid2] = getCanvasIntersectionPoint(thisTri.v2() + *currentModel->getPos());
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

//angleZ is the angle from the xz plane zHat takes,
//angleX is the angle from the xy plane xHat takes
void Camera::rot(float angleZ, float angleX) {
    this->orientation = Utils::pitch(angleZ) * this->orientation; //does correct pitch rotation
    //rotate each camera axis around the world y respectively
    glm::mat3 yaw = Utils::yaw(angleX); //rotation matrix
    glm::vec3 camX = yaw * glm::vec3(this->orientation[0].x, this->orientation[1].x, this->orientation[2].x);
    glm::vec3 camY = yaw * glm::vec3(this->orientation[0].y, this->orientation[1].y, this->orientation[2].y);
    glm::vec3 camZ = yaw * glm::vec3(this->orientation[0].z, this->orientation[1].z, this->orientation[2].z);
    this->orientation[0] = {camX.x, camY.x, camZ.x};
    this->orientation[1] = {camX.y, camY.y, camZ.y};
    this->orientation[2] = {camX.z, camY.z, camZ.z};
}


void Camera::setRot(float angleX, float angleY) {
    this->orientation = glm::mat3({1, 0, 0},
                                  {0, 1, 0},
                                  {0, 0, 1});
    rot(angleX, angleY);
}

void Camera::lookAt(glm::vec3 at) {
    glm::vec3 direction = this->position - at;
    this->orientation = Utils::rotateMeTo(direction, glm::vec3(0, 1, 0));
}

glm::vec3 Camera::getPos() {
    return this->position;
}

glm::vec2 Camera::getRot(){ //rot round x, rot round y, careful when y is +-90
//    glm::vec3 lastRow = this->orientation[2];
//    return glm::vec2(glm::atan(lastRow.y, lastRow.z),
//                     glm::atan(-lastRow.y, glm::sqrt((lastRow.y * lastRow.y) + (lastRow.z * lastRow.z))));
    glm::vec3 xHat = this->myRight();
    glm::vec3 zHat = this->myFwd();
    float xSign = 1;
    float zSign = 1;
    if(xHat.z != 0) xSign = xHat.z / glm::abs(xHat.z);//sign on whether its behind or in front of the camera
    if(zHat.y != 0) zSign = zHat.y / glm::abs(zHat.y); //sign on whether its
    // projection vectors
    glm::vec3 xHat0z = this->myRight(); xHat0z.z = 0;
    glm::vec3 zHat0y = this->myFwd(); zHat0y.y = 0;
    // angles
    xHat = glm::normalize(xHat);
    zHat = glm::normalize(zHat);
    xHat0z = glm::normalize(xHat0z);
    zHat0y = glm::normalize(zHat0y);
    float tX = (glm::acos(glm::dot(xHat, xHat0z))) * xSign; //signed angle from xhat to world x
    float tZ = (glm::acos(glm::dot(zHat, zHat0y))) * zSign; //signed angle form zhat to world z

    return glm::vec2(-tZ, tX);
}

//normalised vector
glm::vec3 Camera::myUp() {
    return glm::normalize(glm::vec3(this->orientation[0].y,
                     this->orientation[1].y,
                     this->orientation[2].y));
}

glm::vec3 Camera::myFwd(){
    return {this->orientation[0].z, this->orientation[1].z, this->orientation[2].z};
}

glm::vec3 Camera::myRight(){
    return {this->orientation[0].x, this->orientation[1].x, this->orientation[2].x};
}


void Camera::toggleOrbit() {
    this->isOrbiting = !this->isOrbiting;
}

void Camera::doOrbit(ModelLoader model) {
    if(isOrbiting){
        glm::vec3 modelCentre = *model.getPos();
        glm::vec3 toModel = this->position - modelCentre;
        this->position = modelCentre + (Utils::yaw(0.01) * toModel); //rotate then translate
        this->lookAt(modelCentre);
    }
}

void Camera::renderMode() {
    this->mode = (this->mode + 1) % 3;
}

void Camera::doRaytracing(DrawingWindow& window) {
    if(this->mode == ray){

        auto f = [&, me = this](int start, int end){
            me->raycast(window, start, end);
        };

        std::vector<std::thread> slices = {};
        for(int i = 0; i < this->threads; i++){
            //slices.push_back(std::thread(&Camera::raycast, this, std::ref(window), this->sliceHeights[i] + 1, this->sliceHeights[i + 1]));
            slices.push_back(std::thread(f, this->sliceHeights[i], this->sliceHeights[i + 1]));
        }

        for(std::thread &t : slices){
            t.join();
        }
    }
}

void Camera::doRasterising(DrawingWindow &window, DepthBuffer &depthBuffer){
    if(this->mode == rst || this->mode == msh){
        rasterise(window,  depthBuffer);
    }
}

void Camera::setRenderMode(int m) {
    this->mode = m % 3;
}

