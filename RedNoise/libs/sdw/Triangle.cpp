//
// Created by elliot on 09/10/23.
//

#include "Triangle.h"
#include "Line.h"
#include "Utils.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <tuple>
#include <algorithm>

using namespace std;
using namespace glm;

Triangle::Triangle(CanvasTriangle tri, Colour &colour) {
    this->tri = tri;
    this->colour = colour;
    this->hasTexture = false;
}

Triangle::Triangle(CanvasTriangle tri, Colour &colour, TextureMap& texture) {
    this->tri = tri;
    this->vt0 = vec2(tri.v0().x, tri.v0().y); //default set texture vertices to triangle vertices
    this->vt1 = vec2(tri.v1().x, tri.v1().y);
    this->vt2 = vec2(tri.v2().x, tri.v2().y);
    this->colour = colour;
    this->texture = texture;
    this->hasTexture = true;
}

Triangle::Triangle(CanvasTriangle tri, Colour &colour, TextureMap& texture, CanvasTriangle textureTri) {
    this->tri = tri;
    this->vt0 = vec2(textureTri.v0().x, textureTri.v0().y); //default set texture vertices to triangle vertices
    this->vt1 = vec2(textureTri.v1().x, textureTri.v1().y);
    this->vt2 = vec2(textureTri.v2().x, textureTri.v2().y);
    this->colour = colour;
    this->texture = texture;
    this->hasTexture = true;
}


Triangle::Triangle() {
    this->tri = randomCanvasTriangle();
    Colour* randColour = new Colour(random() % 255, random() % 255, random() % 255);
    this->colour = *randColour;
    this->hasTexture = false;
}

Triangle::Triangle(TextureMap& texture){
    this->tri = randomCanvasTriangle();
    this->texture = texture;
    this->vt0 = vec2(tri.v0().x, tri.v0().y); //default set texture vertices to triangle vertices
    this->vt1 = vec2(tri.v1().x, tri.v1().y);
    this->vt2 = vec2(tri.v2().x, tri.v2().y);
    Colour* randColour = new Colour(random() % 255, random() % 255, random() % 255);
    this->colour = *randColour;
    this->hasTexture = true;
}

const void Triangle::draw(DrawingWindow &window) {
    CanvasPoint v0 = tri.v0();
    CanvasPoint v1 = tri.v1();
    CanvasPoint v2 = tri.v2();

    Line::draw(window, vec2(v0.x, v0.y), vec2(v1.x, v1.y), colour, 1);
    Line::draw(window, vec2(v1.x, v1.y), vec2(v2.x, v2.y), colour, 1);
    Line::draw(window, vec2(v2.x,v2.y), vec2(v0.x, v0.y), colour, 1);
}

tuple<vec2, vec2, vec2, vec2> Triangle::splitTriangle(vector<vec2> vs){
    //highest up the image (lowest y value) to lowest down the image (highest y value)

    vec2 vTop = vs[0];
    vec2 vMid = vs[1];
    vec2 vBot = vs[2];
    vec2 vSide = vBot - vTop;
    float hTop = vMid.y - vTop.y; //height of top triangle (number of interpolation steps)
    float coef = hTop / vSide.y;
    vec2 vNew = vTop + (vSide * coef);

    return {vTop, vNew, vMid, vBot};
}

tuple<vector<float>, vector<float>> Triangle::interpolateTwoSides(vec2 vPoint, vec2 vA, vec2 vB, int lines){
    vector<float> sideA;
    vector<float> sideB;
    if(lines > 0){
        sideA = Utils::interpolateSingleFloats(vPoint.x, vA.x, lines);
        sideB = Utils::interpolateSingleFloats(vPoint.x, vB.x, lines);
    }
    return {sideA, sideB};
}

const void Triangle::fill(DrawingWindow &window) {
    CanvasPoint unpack0 = tri.v0();
    CanvasPoint unpack1 = tri.v1();
    CanvasPoint unpack2 = tri.v2();
    vec2 v0 = vec2(unpack0.x, unpack0.y);
    vec2 v1 = vec2(unpack1.x, unpack1.y);
    vec2 v2 = vec2(unpack2.x, unpack2.y);
    vector<vec2> vs = {v0, v1, v2};
    std::sort(vs.begin(), vs.end(), [] (const vec2& v0, const vec2& v1) -> bool {return v0.y < v1.y;}); //highest to lowest

    auto [vTop, vNew, vSplit, vBottom] = splitTriangle(vs);
    float hTop = vSplit.y - vTop.y; //height of top triangle (number of interpolation steps)
    float hBottom = vBottom.y - vSplit.y; //height of bottom triangle (number of interpolation steps)

    int topLines = static_cast<int>(ceil(hTop));
    int topY = static_cast<int>(round(vTop.y));
    auto [topSideA, topSideB] = interpolateTwoSides(vTop, vSplit, vNew, topLines);

    int bottomLines = static_cast<int>(ceil(hBottom));
    int bottomY = static_cast<int>(round(vBottom.y));
    auto [bottomSideA, bottomSideB] = interpolateTwoSides(vBottom, vSplit, vNew, bottomLines);

    for(int i = 0; i < topLines; i++){
        Line::draw(window, vec2(round(topSideA[i]), topY + i + 1), vec2(round(topSideB[i]), topY + i + 1), this->colour, 1);
    }
    for(int i = 0; i < bottomLines; i++){
        Line::draw(window, vec2(round(bottomSideA[i]), bottomY - i - 1), vec2(round(bottomSideB[i]), bottomY - i - 1), this->colour, 1);
    }
    this->drawOutline(window);
}

const void Triangle::drawWithTexture(DrawingWindow &window){//, vec2 vt0, vec2 vt1, vec2 vt2) {
    if(!this->hasTexture) throw invalid_argument("drawWithTexture: triangle has no texture");

    CanvasPoint unpack0 = tri.v0();
    CanvasPoint unpack1 = tri.v1();
    CanvasPoint unpack2 = tri.v2();
    vec2 v0 = vec2(unpack0.x, unpack0.y);
    vec2 v1 = vec2(unpack1.x, unpack1.y);
    vec2 v2 = vec2(unpack2.x, unpack2.y);

    vector<tuple<vec2, vec2>> vsAndVts = {{v0, this->vt0}, {v1, this->vt1}, {v2, this->vt2}};
    //sort vertices and texture vertices together to keep winding consistent
    std::sort(vsAndVts.begin(), vsAndVts.end(), [] (const tuple<vec2, vec2>& v0vt0, const tuple<vec2, vec2>& v1vt1) -> bool
        {return get<0>(v0vt0).y < get<0>(v1vt1).y;}); //highest to lowest
    vector<vec2> vs;
    for(tuple<vec2, vec2> v: vsAndVts) vs.push_back(get<0>(v));

    auto [vTop, vNew, vMid, vBot] = splitTriangle(vs);
    float hTop = vMid.y - vTop.y; //height of top triangle (number of interpolation steps)
    float hBottom = vBot.y - vMid.y; //height of bottom triangle (number of interpolation steps)

    int topLines = static_cast<int>(ceil(hTop));
    int topY = static_cast<int>(round(vTop.y));
    auto [topSideA, topSideB] = interpolateTwoSides(vTop, vMid, vNew, topLines);

    int bottomLines = static_cast<int>(ceil(hBottom));
    int bottomY = static_cast<int>(round(vBot.y));
    auto [bottomSideA, bottomSideB] = interpolateTwoSides(vBot, vMid, vNew, bottomLines);
    vec4 middleLine;
    middleLine = vec4(vec2(round(vNew.x), vNew.y), vec2(round(vMid.x), vMid.y));

    vector<vec2> vts;
    for(tuple<vec2, vec2> v: vsAndVts) vts.push_back(get<1>(v)); //unpack texture vertices sorted which correspond to the triangle vertices
    vec2 vtTop = vts[0];
    vec2 vtMid = vts[1];
    vec2 vtBot = vts[2];
    float fracDownLongSide = (vMid.y - vTop.y) / (vBot.y - vTop.y); //used to place vSplit for texture triangle
    vec2 vtNew = vtTop + (fracDownLongSide * (vtBot - vtTop));
    vector<vec2> topSideTextureA;
    vector<vec2> topSideTextureB;
    if(topLines > 0){
        topSideTextureA = Utils::interpolateTwoElementValues(vtTop, vtMid, topLines); //same number of texture lines as display lines
        topSideTextureB = Utils::interpolateTwoElementValues(vtTop, vtNew, topLines);
    }
    vector<vec2> bottomSideTextureA;
    vector<vec2> bottomSideTextureB;
    if(bottomLines > 0){
        bottomSideTextureA = Utils::interpolateTwoElementValues(vtBot, vtMid, bottomLines);
        bottomSideTextureB = Utils::interpolateTwoElementValues(vtBot, vtNew, bottomLines);
    }


    for(int i = 0; i < topLines; i++){
        const int xA = static_cast<int>(round(topSideA[i]));
        const int y = topY + i + 1; // offset stops it drawing the same line twice
        const int xB = static_cast<int>(round(topSideB[i]));
        Line::draw(window, vec2(xA, y), vec2(xB, y), round(topSideTextureA[i]), round(topSideTextureB[i]), this->texture, 1);
    }
    for(int i = 0; i < bottomLines; i++){
        const int xA = static_cast<int>(round(bottomSideA[i]));
        const int y = bottomY - i - 1; // offset stops it drawing the same line twice
        const int xB = static_cast<int>(round(bottomSideB[i]));
        Line::draw(window, vec2(xA, y), vec2(xB, y), round(bottomSideTextureA[i]), round(bottomSideTextureB[i]), this->texture, 1);
    }
    this->drawOutline(window);
}

CanvasTriangle Triangle::randomCanvasTriangle() {
    int WIDTH = 320;
    int HEIGHT = 240;
    CanvasPoint* v0 = new CanvasPoint(random() % WIDTH, random() % HEIGHT);
    CanvasPoint* v1 = new CanvasPoint(random() % WIDTH, random() % HEIGHT);
    CanvasPoint* v2 = new CanvasPoint(random() % WIDTH, random() % HEIGHT);
    if(v0->x == v1->x) v1->x += 10;
    if(v0->x == v2->x) v2->x -= 10;
    cout << v0->x << ':' << v0->y << ',' << v1->x << ':' << v1->y << ',' << v2->x << ':' << v2->y << endl;
    return *new CanvasTriangle(*v0, *v1, *v2);
}

void Triangle::drawOutline(DrawingWindow &window) {
    (new Triangle(this->tri, *new Colour(255, 255, 255)))->draw(window);
}
