//
// Created by elliot on 09/10/23.
//

#include "Triangle.h"
#include "Line.h"
#include "Utils.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <algorithm>

using namespace std;
using namespace glm;

Triangle::Triangle(CanvasTriangle tri, Colour &colour) {
    this->tri = tri;
    this->colour = colour;
}

const void Triangle::draw(DrawingWindow &window) {
    CanvasPoint v0 = tri.v0();
    CanvasPoint v1 = tri.v1();
    CanvasPoint v2 = tri.v2();

    Line::draw(window, vec2(v0.x, v0.y), vec2(v1.x, v1.y), colour, 1);
    Line::draw(window, vec2(v1.x, v1.y), vec2(v2.x, v2.y), colour, 1);
    Line::draw(window, vec2(v2.x,v2.y), vec2(v0.x, v0.y), colour, 1);
}

Triangle::Triangle() {
    int WIDTH = 320;
    int HEIGHT = 240;
    Colour* colour = new Colour(random() % 255, random() % 255, random() % 255);
    CanvasPoint* v0 = new CanvasPoint(random() % WIDTH, random() % HEIGHT);
    CanvasPoint* v1 = new CanvasPoint(random() % WIDTH, random() % HEIGHT);
    CanvasPoint* v2 = new CanvasPoint(random() % WIDTH, random() % HEIGHT);
    this->tri = CanvasTriangle(*v0, *v1, *v2);
    this->colour = *colour;
}

const void Triangle::fill(DrawingWindow &window) {
    CanvasPoint unpack0 = tri.v0();
    CanvasPoint unpack1 = tri.v1();
    CanvasPoint unpack2 = tri.v2();
    vec2 v0 = vec2(unpack0.x, unpack0.y);
    vec2 v1 = vec2(unpack1.x, unpack1.y);
    vec2 v2 = vec2(unpack2.x, unpack2.y);
    //vertical range of the sides
    float r0 = abs(v1.y - v0.y);
    float r1 = abs(v2.y - v1.y);
    float r2 = abs(v0.y - v2.y);
    float rMax = Utils::max(Utils::max(r0, r1), r2); //we just need the max to find the two smallest ones
    //our split vertex
    vec2 vSplit;
    vec4 sideAndOffset; //the vector of the tallest side(for interpolating) and the highest side for an vTop
    if(rMax == r0) {
        vSplit = v2;
        if(v0.y < v1.y) sideAndOffset = vec4(v1 - v0, v0);
        else sideAndOffset = vec4(v0 - v1, v1);
    } //common vertex
    if(rMax == r1) {
        vSplit = v0;
        if(v1.y < v2.y) sideAndOffset = vec4(v2 - v1, v1);
        else sideAndOffset = vec4(v1 - v2, v2);
    }
    if(rMax == r2) {
        vSplit = v1;
        if(v0.y < v2.y) sideAndOffset = vec4(v2 - v0, v0);
        else sideAndOffset = vec4(v0 - v2, v2);
    }
    vec2 side = vec2(sideAndOffset.x, sideAndOffset.y);
    vec2 vTop = vec2(sideAndOffset.z, sideAndOffset.w);
    vec2 vBottom = side + vTop;
    float hTop = vSplit.y - sideAndOffset.w; //height of top triangle (number of interpolation steps)
    float hBottom = vBottom.y - vSplit.y; //height of bottom triangle (number of interpolation steps)
    float coef = (hTop) / sideAndOffset.y;
    vec2 vNew = vTop + (side * coef);
//  DEBUGGING
//    Triangle* tTop = new Triangle(
//            *new CanvasTriangle(
//                    *new CanvasPoint(vTop.x, vTop.y),
//                    *new CanvasPoint(vNew.x, vNew.y),
//                    *new CanvasPoint(vSplit.x, vSplit.y)
//                    ), *new Colour(255, 0, 0));
//    Triangle* tBottom = new Triangle(
//            *new CanvasTriangle(
//                    *new CanvasPoint(vBottom.x, vBottom.y),
//                    *new CanvasPoint(vNew.x, vNew.y),
//                    *new CanvasPoint(vSplit.x, vSplit.y)
//            ), *new Colour(0, 255, 0));
//
//    //tTop->draw(window);
//    //tBottom->draw(window);

    int topLines = static_cast<int>(ceil(hTop));
    int topY = static_cast<int>(round(vTop.y));
    vector<float> topSideA;
    vector<float> topSideB;
    if(topLines > 0){
        topSideA = Utils::interpolateSingleFloats(vTop.x, vSplit.x, topLines);
        topSideB = Utils::interpolateSingleFloats(vTop.x, vNew.x, topLines);
    }

    int bottomLines = static_cast<int>(ceil(hBottom));
    int bottomY = static_cast<int>(round(vBottom.y));
    vector<float> bottomSideA;
    vector<float> bottomSideB;
    if(bottomLines > 0) {
        bottomSideA = Utils::interpolateSingleFloats(vBottom.x, vSplit.x, bottomLines);
        bottomSideB = Utils::interpolateSingleFloats(vBottom.x, vNew.x, bottomLines);
    }
    //fill those holes!
    vec4 middleLine;
    if(vSplit.x > vNew.x){
        middleLine = vec4(vec2(floor(vNew.x), vNew.y), vec2(ceil(vSplit.x), vSplit.y));
        for(int i = 0; i < topLines; i++){
            topSideA[i] = round(topSideA[i]);
            topSideB[i] = round(topSideB[i]);
        }
        for(int i = 0; i < bottomLines; i++) {
            bottomSideA[i] = round(bottomSideA[i]);
            bottomSideB[i] = round(bottomSideB[i]);
        }
    }else{
        middleLine = vec4(vec2(ceil(vNew.x), vNew.y), vec2(floor(vSplit.x), vSplit.y));
        for(int i = 0; i < topLines; i++){
            topSideA[i] = round(topSideA[i]);
            topSideB[i] = round(topSideB[i]);
        }
        for(int i = 0; i < bottomLines; i++){
            bottomSideA[i] = round(bottomSideA[i]);
            bottomSideB[i] = round(bottomSideB[i]);
        }
    }

    for(int i = 0; i < topLines; i++){
        Line::draw(window, vec2(topSideA[i], topY + i), vec2(topSideB[i], topY + i), this->colour, 1);
    }
    Line::draw(window, vec2(middleLine.x, middleLine.y), vec2(middleLine.z, middleLine.w), this->colour, 1);
    for(int i = 0; i < bottomLines; i++){
        Line::draw(window, vec2(bottomSideA[i], bottomY - i), vec2(bottomSideB[i], bottomY - i), this->colour, 1);
    }
    (new Triangle(this->tri, *new Colour(255, 255, 255)))->draw(window);
}
