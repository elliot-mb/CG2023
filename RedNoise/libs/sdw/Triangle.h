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
    void drawOutline(DrawingWindow& window);
private:
    //vtop vnew vsplit vbottom
    std::tuple<glm::vec2, glm::vec2, glm::vec2, glm::vec2> splitTriangle(std::vector<glm::vec2> vs);
    //generates two lists of x coordinates along the two sides which are being interpolated
    std::tuple<std::vector<float>, std::vector<float>> interpolateTwoSides(glm::vec2 vPoint, glm::vec2 vA, glm::vec2 vB, int lines);
    static CanvasTriangle randomCanvasTriangle();

    CanvasTriangle tri;
    glm::vec2 vt0; //texture vertices
    glm::vec2 vt1;
    glm::vec2 vt2;
    Colour colour;
    TextureMap texture;
    bool hasTexture;
};

