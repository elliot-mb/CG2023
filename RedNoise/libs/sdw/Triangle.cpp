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
    vector<vec2> pts0 = Line::pixels(v0, v1);
    vector<vec2> pts1 = Line::pixels(v1, v2);
    vector<vec2> pts2 = Line::pixels(v2, v0);
    std::sort(pts0.begin(), pts0.end(), [](vec2 a, vec2 b){ return a.y < b.y; });
    std::sort(pts1.begin(), pts1.end(), [](vec2 a, vec2 b){ return a.y < b.y; });
    std::sort(pts2.begin(), pts2.end(), [](vec2 a, vec2 b){ return a.y < b.y; });
    //vertical range of the sides
    float r0 = abs(v1.y - v0.y);
    float r1 = abs(v2.y - v1.y);
    float r2 = abs(v0.y - v2.y);
    float rMax = Utils::max(Utils::max(r0, r1), r2); //we just need the max to find the two smallest ones
    //our split vertex
    vec2 vSplit;
    if(rMax == r0) vSplit = v2; //common vertex
    if(rMax == r1) vSplit = v0;
    if(rMax == r2) vSplit = v1;

//    //split out the x values into separate arrays
//    vector<float> pts0x = {}; //they will be sorted on their y values
//    for(int i = 0; i < pts0.size(); i++) pts0x.push_back(pts0[i].x);
//    float pts0xOffset = Utils::min(v0.y, v1.y); //where does the sorted array start? we can use this to turn y values into indices
//    vector<float> pts1x = {}; //they will be sorted on their y values
//    for(int i = 0; i < pts1.size(); i++) pts1x.push_back(pts1[i].x);
//    float pts1xOffset = Utils::min(v1.y, v2.y); //where does the sorted array start? we can use this to turn y values into indices
//    vector<float> pts2x = {}; //they will be sorted on their y values
//    for(int i = 0; i < pts2.size(); i++) pts2x.push_back(pts2[i].x);
//    float pts2xOffset = Utils::min(v0.y, v2.y); //where does the sorted array start? we can use this to turn y values into indices
//
//    if(vSplit == v2){ //we KNOW v0 -> v1 (pts0) has the greatest number of points
//        Line::draw(window, v0, v1, *new Colour(255, 255, 255), 1);
//        if(v0.y < v1.y){ //then v2 connects to v0, where v0 is the top of the triangle
//            int lastY = 0;
//            for(int i = 0; pts2[i].y < v2.y; i++) {
//                if(pts2[i].y != lastY) Line::draw(window, pts0[i], pts2[i], this->colour, 1);
//                lastY = pts2[i].y;
//            }
//        }else{ //then v2 connects to v1, where v1 is the top of the triangle
//            for(int i = 0; pts1[i].y < v2.y; i++) {
//                Line::draw(window, pts0[i], pts1[i], this->colour, 1);
//            }
//        }
//    }
//    if(vSplit == v0){ //we KNOW v1 -> v2 (pts1) has the greatest number of points
//        Line::draw(window, v1, v2, *new Colour(255, 255, 255), 1);
//    }
//    if(vSplit == v1){ //we KNOW v0 -> v2 (pts2) has the greatest number of points
//        Line::draw(window, v0, v2, *new Colour(255, 255, 255), 1);
//    }
////    if(vSplit.x == v2.x && vSplit.y == v2.y) { //r0 is the largest (most vertical) side, but which way up is it?
////        if(v0.y < v1.y){
////            for(int i = 0; i < pts2.size(); i++){
////                Line::draw(window, pts0[i], pts2[i], this->colour, 1);
////            }
////            for(int i = 0; i + pts2.size() < pts0.size() && i < pts1.size(); i++){
////                Line::draw(window, pts0[i + pts2.size()], pts1[i], this->colour, 1);
////            }
////        }else{
////            for(int i = 0; i < pts1.size(); i++){
////                Line::draw(window, pts0[i], pts1[i], this->colour, 1);
////            }
////            for(int i = 0; i + pts2.size() < pts0.size() && i < pts1.size(); i++){
////                Line::draw(window, pts0[i + pts1.size()], pts2[i], this->colour, 1);
////            }
////        }
////    }
}
