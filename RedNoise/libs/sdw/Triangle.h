//
// Created by elliot on 09/10/23.
//
#include "CanvasTriangle.h"
#include "DrawingWindow.h"
#include "Colour.h"
#include "TextureMap.h"
#include <tuple>

#pragma once


class Triangle {
public:
    Triangle(); //new random triangle
    Triangle(TextureMap &texture);
    Triangle(CanvasTriangle tri, Colour &colour, TextureMap &texture);
    Triangle(CanvasTriangle tri, Colour &colour);

    Triangle(CanvasTriangle tri, Colour &colour, TextureMap &texture, CanvasTriangle textureTri);

    void draw(DrawingWindow& window); //just needs the window to draw tri
    void fill(DrawingWindow& window);
    void fillTexture(DrawingWindow& window);
private:
    //vtop vnew vsplit vbottom
    tuple<vec2, vec2, vec2, vec2> splitTriangle(vector<vec2> vs);
    //generates two lists of x coordinates along the two sides which are being interpolated
    tuple<vector<float>, vector<float>> interpolateTwoSides(vec2 vPoint, vec2 vA, vec2 vB, int lines);
    static CanvasTriangle randomCanvasTriangle();
    void drawOutline(DrawingWindow& window);

    CanvasTriangle tri;
    vec2 vt0; //texture vertices
    vec2 vt1;
    vec2 vt2;
    Colour colour;
    TextureMap texture;
    bool hasTexture;
};

