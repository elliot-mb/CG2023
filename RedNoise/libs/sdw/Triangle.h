//
// Created by elliot on 09/10/23.
//
#include <CanvasTriangle.h>
#include "DrawingWindow.h"
#include "Colour.h"

#pragma once


class Triangle {
public:
    Triangle(); //new random triangle
    Triangle(CanvasTriangle tri, Colour &colour);

    const void draw(DrawingWindow& window); //just needs the window to draw tri
    const void fill(DrawingWindow& window);
private:
    CanvasTriangle tri;
    Colour colour;

};

