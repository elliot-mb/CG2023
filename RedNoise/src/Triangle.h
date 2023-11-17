//
// Created by elliot on 09/10/23.
//
#include "CanvasTriangle.h"
#include "DrawingWindow.h"
#include "Colour.h"
#include "TextureMap.h"
#include "DepthBuffer.h"
#include <tuple>

#pragma once


class Triangle {
public:
    Triangle(); //new random triangle
    explicit Triangle(TextureMap &texture);
    Triangle(glm::mat3 tri3, Colour &colour);
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour &colour);
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour &colour, TextureMap &texture);
    Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour &colour, TextureMap &texture, CanvasTriangle textureTri);

    bool isTextured();
    void draw(DrawingWindow& window); //just needs the window to draw tri
    void fill(DrawingWindow& window, DepthBuffer& db);
    void fillTexture(DrawingWindow& window, DepthBuffer& db);
    void drawOutline(DrawingWindow& window, Colour colour);
    glm::vec3 v0();
    void setV0(glm::vec3 v0);
    glm::vec3 v1();
    void setV1(glm::vec3 v1);
    glm::vec3 v2();
    void setV2(glm::vec3 v2);
    Colour& getColour();
    glm::vec3* getNormal();

    void setN0(glm::vec3 nrm0);
    void setN1(glm::vec3 nrm1);
    void setN2(glm::vec3 nrm2);
    glm::vec3* n0();
    glm::vec3* n1();
    glm::vec3* n2();

    glm::vec3 e0;
    glm::vec3 e1;
private:
    //vtop vnew vsplit vbottom
    std::tuple<glm::vec3, glm::vec3, glm::vec3, glm::vec3> splitTriangle(std::vector<glm::vec3> vs);
    //generates two lists of x coordinates along the two sides which are being interpolated
    std::tuple<std::vector<float>, std::vector<float>> interpolateTwoSides(glm::vec2 vPoint, glm::vec2 vA, glm::vec2 vB, int lines);
    static glm::mat3 randomCanvasTriangle();

    glm::mat3 tri3; //triangle in 3d space
    glm::vec2 vt0; //texture vertices
    glm::vec2 vt1;
    glm::vec2 vt2;
    glm::vec3 nrm0; //vertex normals
    glm::vec3 nrm1;
    glm::vec3 nrm2;


    glm::vec3 normal;
    Colour colour;
    TextureMap texture;
    bool hasTexture;
};

