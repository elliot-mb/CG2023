//
// Created by elliot on 09/10/23.
//

#include "Triangle.h"
#include "Line.h"
#include "Utils.h"
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "NormalMap.h"
#include <tuple>
#include <algorithm>

Triangle::Triangle(glm::mat3 tri3, Colour &colour) {
    this->tri3 = glm::mat3(tri3[0], tri3[1], tri3[2]);
    this->colour = colour;
    this->hasTexture = false;
    this->e0 = this->v1() - this->v0();
    this->e1 = this->v2() - this->v0();
    this->normal = glm::normalize(glm::cross(this->v2() - this->v1(), this->v0() - this->v1()));
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour &colour){
    this->tri3 = glm::mat3(v0, v1, v2);
    this->colour = colour;
    this->hasTexture = false;
    this->e0 = this->v1() - this->v0();
    this->e1 = this->v2() - this->v0();
    this->normal = glm::normalize(glm::cross(this->v2() - this->v1(), this->v0() - this->v1()));
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour &colour, TextureMap& texture){
    this->tri3 = glm::mat3(v0, v1, v2);
    this->vt0 = glm::vec2(tri3[0].x, tri3[0].y); //default set texture vertices to triangle vertices
    this->vt1 = glm::vec2(tri3[1].x, tri3[1].y);
    this->vt2 = glm::vec2(tri3[2].x, tri3[3].y);
    this->colour = colour;
    this->texture = texture;
    this->hasTexture = true;
    this->e0 = this->v1() - this->v0();
    this->e1 = this->v2() - this->v0();
    this->normal = glm::normalize(glm::cross(this->v2() - this->v1(), this->v0() - this->v1()));
}

Triangle::Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour &colour, TextureMap& texture, CanvasTriangle textureTri, NormalMap& nm, bool hasNormalMap) {
    this->tri3 = glm::mat3(v0, v1, v2);
    this->vt0 = glm::vec2(textureTri.v0().x, textureTri.v0().y); //default set texture vertices to triangle vertices
    this->vt1 = glm::vec2(textureTri.v1().x, textureTri.v1().y);
    this->vt2 = glm::vec2(textureTri.v2().x, textureTri.v2().y);
    this->colour = colour;
    this->texture = texture;
    this->hasTexture = !hasNormalMap;
    this->e0 = this->v1() - this->v0();
    this->e1 = this->v2() - this->v0();
    this->normal = glm::normalize(glm::cross(this->v2() - this->v1(), this->v0() - this->v1()));
    this->normalMap = nm;
    this->hasNormalMap = hasNormalMap;
    if(hasNormalMap){
        glm::vec2 ll = Utils::latLong(this->normal);
        this->normRot = Utils::yaw(ll.y) * Utils::pitch(ll.x);
    }
}


Triangle::Triangle() {
    //this->tri = randomCanvasTriangle();
    Colour* randColour = new Colour(random() % 255, random() % 255, random() % 255);
    this->colour = *randColour;
    this->hasTexture = false;
}

Triangle::Triangle(TextureMap& texture){
    this->tri3 = randomCanvasTriangle();
    this->texture = texture;
    this->vt0 = glm::vec2(tri3[0].x, tri3[0].y); //default set texture vertices to triangle vertices
    this->vt1 = glm::vec2(tri3[1].x, tri3[1].y);
    this->vt2 = glm::vec2(tri3[2].x, tri3[3].y);
    Colour* randColour = new Colour(random() % 255, random() % 255, random() % 255);
    this->colour = *randColour;
    this->hasTexture = true;
}

void Triangle::draw(DrawingWindow &window) {
    glm::vec2 v0 = glm::vec2(this->tri3[0].x, this->tri3[0].y);
    glm::vec2 v1 = glm::vec2(this->tri3[1].x, this->tri3[1].y);
    glm::vec2 v2 = glm::vec2(this->tri3[2].x, this->tri3[2].y);

    Line::draw(window, glm::vec2(v0.x, v0.y), glm::vec2(v1.x, v1.y), colour, 1);
    Line::draw(window, glm::vec2(v1.x, v1.y), glm::vec2(v2.x, v2.y), colour, 1);
    Line::draw(window, glm::vec2(v2.x,v2.y), glm::vec2(v0.x, v0.y), colour, 1);
}

std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3> Triangle::splitTriangle(std::vector<glm::vec3> vs){
    //highest up the image (lowest y value) to lowest down the image (highest y value)

    glm::vec3 vTop = vs[0];
    glm::vec3 vMid = vs[1];
    glm::vec3 vBot = vs[2];
    glm::vec3 vSide = vBot - vTop;
    float hTop = vMid.y - vTop.y; //height of top triangle (number of interpolation steps)
    float fracDownLongSide = hTop / vSide.y;
    glm::vec3 vNew = vTop + (vSide * fracDownLongSide);

    return {vTop, vNew, vMid, vBot};
}

std::tuple<std::vector<float>, std::vector<float>> Triangle::interpolateTwoSides(glm::vec2 vPoint, glm::vec2 vA, glm::vec2 vB, int lines){
    std::vector<float> sideA;
    std::vector<float> sideB;

    if(lines > 0){
        sideA = Utils::interpolateSingleFloats(vPoint.x, vA.x, lines);
        sideB = Utils::interpolateSingleFloats(vPoint.x, vB.x, lines);
    }
    return {sideA, sideB};
}

void Triangle::fill(DrawingWindow &window, DepthBuffer& db) {
    std::vector<glm::vec3> vs = {v0(), v1(), v2()};
    int outside = 0;
//    for(glm::vec3 v : vs){
//        if (v.x < 0 || v.x > static_cast<float>(window.width) || v.y < 0 || v.y > static_cast<float>(window.height)){
//            outside++;
//        }
//    }
//    if(outside == static_cast<int>(vs.size())) return; //are all the the vertices outside of the screen

    std::sort(vs.begin(), vs.end(), [] (const glm::vec3& v0, const glm::vec3& v1) -> bool {return v0.y < v1.y;}); //highest to lowest

    auto [vTop, vNew, vSplit, vBottom] = splitTriangle(vs);
    float hTop = vSplit.y - vTop.y; //height of top triangle (number of interpolation steps)
    float hBottom = vBottom.y - vSplit.y; //height of bottom triangle (number of interpolation steps)

    float totalLines = hTop + hBottom;

    int topLines = static_cast<int>(ceil(hTop)) + 1;
    int bottomLines = static_cast<int>(ceil(hBottom)) + 1;
    if(topLines > 0) {
        auto [topSideA, topSideB] = interpolateTwoSides(glm::vec2(vTop), glm::vec2(vSplit), glm::vec2(vNew), topLines);
        std::vector<float> depthsTopSideA = Utils::interpolateSingleFloats(vTop.z, vSplit.z, topLines);
        std::vector<float> depthsTopSideB = Utils::interpolateSingleFloats(vTop.z, vNew.z, topLines);
        for(int i = 0; i < topLines; i++){
            float xA = topSideA[i];
            float y = vTop.y + static_cast<float>(i);
            float zA = depthsTopSideA[i];
            float xB = topSideB[i];
            float zB = depthsTopSideB[i];
            Line::draw(window, db, glm::vec3(xA, y, zA), glm::vec3(xB, y, zB), this->colour, 1);
        }
    }
    if(bottomLines > 0){
        auto [bottomSideA, bottomSideB] = interpolateTwoSides(glm::vec2(vBottom), glm::vec2(vSplit), glm::vec2(vNew), bottomLines);
        std::vector<float> depthsBottomSideA = Utils::interpolateSingleFloats(vBottom.z, vSplit.z, bottomLines);
        std::vector<float> depthsBottomSideB = Utils::interpolateSingleFloats(vBottom.z, vNew.z, bottomLines);
        for(int i = 0; i < bottomLines; i++){
            float xA = bottomSideA[i];
            float y = vBottom.y - static_cast<float>(i) + 1;
            float zA = depthsBottomSideA[i];
            float xB = bottomSideB[i];
            float zB = depthsBottomSideB[i];
            Line::draw(window, db, glm::vec3(xA, y, zA), glm::vec3(xB, y, zB), this->colour, 1);
        }
    }

    //this->drawOutline(window, this->colour);
}

void Triangle::fillTexture(DrawingWindow &window, DepthBuffer& db){//, glm::vec2 vt0, glm::vec2 vt1, glm::vec2 vt2) {
    // commented while we get occlusion working
    if(!this->hasTexture) throw std::invalid_argument("fillTexture: triangle has no texture");

    std::vector<std::tuple<glm::vec3, glm::vec2>> vsAndVts = {{v0(), this->vt0}, {v1(), this->vt1}, {v2(), this->vt2}};
    //sort vertices and texture vertices together to keep winding consistent
    std::sort(vsAndVts.begin(), vsAndVts.end(), [] (const std::tuple<glm::vec3, glm::vec2>& v0vt0, const std::tuple<glm::vec3, glm::vec2>& v1vt1) -> bool
        {return std::get<0>(v0vt0).y < std::get<0>(v1vt1).y;}); //highest to lowest
    std::vector<glm::vec3> vs;
    for(std::tuple<glm::vec3, glm::vec2> v: vsAndVts) vs.push_back(std::get<0>(v));

    auto [vTop, vNew, vSplit, vBottom] = splitTriangle(vs);
    float hTop = vSplit.y - vTop.y; //height of top triangle (number of interpolation steps)
    float hBottom = vBottom.y - vSplit.y; //height of bottom triangle (number of interpolation steps)
    float totalLines = hTop + hBottom;

    int topLines = static_cast<int>(ceil(hTop)) + 1;
    int bottomLines = static_cast<int>(ceil(totalLines - static_cast<float>(topLines))) + 2;

    auto [topSideA, topSideB] = interpolateTwoSides(glm::vec2(vTop), glm::vec2(vSplit), glm::vec2(vNew), topLines);
    auto [bottomSideA, bottomSideB] = interpolateTwoSides(glm::vec2(vBottom), glm::vec2(vSplit), glm::vec2(vNew), bottomLines);

    std::vector<glm::vec2> vts;
    for(std::tuple<glm::vec3, glm::vec2> v: vsAndVts) vts.push_back(std::get<1>(v)); //unpack texture vertices sorted which correspond to the triangle vertices
    glm::vec2 vtTop = vts[0]; /// this->v0().z;
    glm::vec2 vtMid = vts[1]; /// this->v1().z;
    glm::vec2 vtBot = vts[2]; /// this->v2().z;
    float fracDownLongSide = (vSplit.y - vTop.y) / (vBottom.y - vTop.y); //used to place vSplit for texture triangle
    glm::vec2 vtNew = vtTop + (fracDownLongSide * (vtBot - vtTop));

    if(topLines > 0){
        std::vector<float> depthsTopSideA = Utils::interpolateSingleFloats(vTop.z, vSplit.z, topLines);
        std::vector<float> depthsTopSideB = Utils::interpolateSingleFloats(vTop.z, vNew.z, topLines);
        std::vector<glm::vec2> topSideTextureA = Utils::interpolateTwoElementValues(vtTop, vtMid, topLines); //same number of texture lines as display lines
        std::vector<glm::vec2> topSideTextureB = Utils::interpolateTwoElementValues(vtTop, vtNew, topLines);
        for(int i = 0; i < topLines; i++){
            float xA = topSideA[i];
            float y =  vTop.y + static_cast<float>(i); // offset stops it drawing the same line twice
            float xB = topSideB[i];
            float zA = depthsTopSideA[i];
            float zB = depthsTopSideB[i];
            Line::draw(window, db,  glm::vec3(xA, y, zA), glm::vec3(xB, y, zB), round(topSideTextureA[i]), round(topSideTextureB[i]), this->texture, 1);
        }
    }
    if(bottomLines > 0){
        std::vector<float> depthsBottomSideA = Utils::interpolateSingleFloats(vBottom.z, vSplit.z, bottomLines);
        std::vector<float> depthsBottomSideB = Utils::interpolateSingleFloats(vBottom.z, vNew.z, bottomLines);
        std::vector<glm::vec2> bottomSideTextureA = Utils::interpolateTwoElementValues(vtBot, vtMid, bottomLines);
        std::vector<glm::vec2> bottomSideTextureB = Utils::interpolateTwoElementValues(vtBot, vtNew, bottomLines);
        for(int i = 0; i < bottomLines; i++){
            float xA = bottomSideA[i];
            float y = vBottom.y - static_cast<float>(i); // offset stops it drawing the same line twice
            float xB = bottomSideB[i];
            float zA = depthsBottomSideA[i];
            float zB = depthsBottomSideB[i];
            Line::draw(window, db,  glm::vec3(xA, y, zA), glm::vec3(xB, y, zB), round(bottomSideTextureA[i]), round(bottomSideTextureB[i]), this->texture, 1);
        }
    }

}

glm::mat3 Triangle::randomCanvasTriangle() {
    int WIDTH = 320;
    int HEIGHT = 240;
    glm::vec2 v0 = glm::vec2(random() % WIDTH, random() % HEIGHT);
    glm::vec2 v1 = glm::vec2(random() % WIDTH, random() % HEIGHT);
    glm::vec2 v2 = glm::vec2(random() % WIDTH, random() % HEIGHT);
    if(v0.x == v1.x) v1.x += 10;
    if(v0.x == v2.x) v2.x -= 10;
    std::cout << v0.x << ':' << v0.y << ',' << v1.x << ':' << v1.y << ',' << v2.x << ':' << v2.y << std::endl;
    return glm::mat3(glm::vec3(v0, 0), glm::vec3(v1, 0), glm::vec3(v2, 0));
}

void Triangle::drawOutline(DrawingWindow &window, Colour aColour) {
    (new Triangle(this->tri3, aColour))->draw(window);
}

glm::vec3 Triangle::v0() {
    return this->tri3[0];
}

glm::vec3 Triangle::v1() {
    return this->tri3[1];
}

glm::vec3 Triangle::v2() {
    return this->tri3[2];
}

Colour& Triangle::getColour() {
    return this->colour;
}

bool Triangle::isTextured() {
    return this->hasTexture;
}

void Triangle::setV0(glm::vec3 v0) {
    this->tri3[0] = v0;
}

void Triangle::setV1(glm::vec3 v1) {
    this->tri3[1] = v1;
}

void Triangle::setV2(glm::vec3 v2) {
    this->tri3[2] = v2;
}

glm::vec3* Triangle::getNormal() {
    return &this->normal;
}

void Triangle::setN0(glm::vec3 n0) {
    this->nrm0 = n0;
}
void Triangle::setN1(glm::vec3 n1) {
    this->nrm1 = n1;
}
void Triangle::setN2(glm::vec3 n2) {
    this->nrm2 = n2;
}
glm::vec3* Triangle::n0() {
    return &this->nrm0;
}
glm::vec3* Triangle::n1() {
    return &this->nrm1;
}
glm::vec3* Triangle::n2() {
    return &this->nrm2;
}

Colour Triangle::getTextureColour(float u, float v, float w) {
    int x = static_cast<int>(glm::floor((this->vt0.x * u) + (this->vt1.x * v) + (this->vt2.x * w)));
    int y = static_cast<int>(glm::floor((this->vt0.y * u) + (this->vt1.y * v) + (this->vt2.y * w)));
    return Utils::unpack(this->texture.pixel(x, y));
}

glm::vec3& Triangle::getNormalMapNormal(float u, float v, float w) {
    int x = static_cast<int>(glm::floor((this->vt0.x * u) + (this->vt1.x * v) + (this->vt2.x * w)));
    int y = static_cast<int>(glm::floor((this->vt0.y * u) + (this->vt1.y * v) + (this->vt2.y * w)));
    return this->normalMap.getNormal(x, y);
}

bool Triangle::isNormalMapped() {
    return this->hasNormalMap;
}

glm::mat3 &Triangle::getNormMapRot() {
    return this->normRot;
}


