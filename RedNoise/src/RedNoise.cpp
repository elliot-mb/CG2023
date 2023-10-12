#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <Line.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <Colour.h>
#include <CanvasPoint.h>
#include "Triangle.h"

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

//GLOBAL!! EVIL!!
vector<Triangle*> triangles = {
        new Triangle(
            *new CanvasTriangle(
                *new CanvasPoint(160, 10),
                *new CanvasPoint(300, 230),
                *new CanvasPoint(10,150)),
            *new Colour(100, 100, 0),
            *new TextureMap("texture.ppm"),
            *new CanvasTriangle(
                    *new CanvasPoint(195, 5),
                    *new CanvasPoint(395, 380),
                    *new CanvasPoint(65,330)))
};

void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			//uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, Utils::pack(255, 127, 127, 127));
		}
	}
}

// template <typename T>

// void showV(vector<T> v) {
// 	for(int i=0; i < v.size(); i++) cout << v[i] << " ";
// 	cout << endl;
// }

void drawGreys(DrawingWindow &window){
	window.clearPixels();
	vector<float> greys = Utils::interpolateSingleFloats(255, 0, window.width);
	vector<uint8_t> greyWholes = {};
	for(int i = 0; i < greys.size(); i++){
		greyWholes.push_back(static_cast<uint8_t>(greys[i]));
	}
	for(int y = 0; y < window.height; y++){
		for(int x = 0; x < window.width; x++){
			uint8_t value = greyWholes[x];
			uint32_t colour = Utils::pack(255, value, value, value);
			window.setPixelColour(x, y, colour);
		}
	}
}

void drawColours(DrawingWindow &window){
	window.clearPixels();
	vec3 red = vec3(255, 0, 0);
	vec3 green = vec3(0, 255, 0);
	vec3 blue = vec3(0, 0, 255);
	vec3 yellow = vec3(255, 255, 0);
	vector<vec3> leftCol = Utils::interpolateThreeElementValues(red, yellow, window.height);
	vector<vec3> rightCol = Utils::interpolateThreeElementValues(blue, green, window.height);
	for(int y = 0; y < window.height; y++){
		vector<vec3> row = Utils::interpolateThreeElementValues(leftCol[y], rightCol[y], window.width);
		for(int x = 0; x < window.width; x++){
			window.setPixelColour(x, y, Utils::pack(
				255, 
				static_cast<uint8_t>(row[x].x),
				static_cast<uint8_t>(row[x].y),
				static_cast<uint8_t>(row[x].z)
			));
		}
	} 
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
        //triangle
        else if (event.key.keysym.sym == SDLK_u) triangles.push_back(new Triangle(*new TextureMap("texture.ppm")));
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	vector<float> result = Utils::interpolateSingleFloats(2.2, 8.5, 7);
	for(int i=0; i < result.size(); i++) cout << result[i] << " ";
	cout << endl;

	vector<vec3> resultVec = Utils::interpolateThreeElementValues(vec3(1.0, 4.0, 9.2), vec3(4.0, 1.0, 9.8), 4);
	for(int i=0; i < resultVec.size(); i++) {
		cout << "(";
		cout << resultVec[i].x << " ";
		cout << resultVec[i].y << " ";
		cout << resultVec[i].z << " ";
		cout << ")";
	}
	cout << endl;

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
        draw(window);
//		//drawColours(window);
//        Colour* c = new Colour();
//		//Line::draw(window, vec2(200.0, 20.0 ), vec2(10.0, 100.0), *c, 1.0);
//        Line::draw(window, vec2(0.0, 0.0), vec2(160.0, 100.0), *c, 1.0);
//        Line::draw(window, vec2(320.0, 0.0), vec2(160.0, 100.0), *c, 1.0);
//        Line::draw(window, vec2(160.0, 0.0), vec2(160, 320), *c, 1.0);
//        Line::draw(window, vec2(80, 100), vec2(240, 100), *c, 1.0);

        for(int i = 0; i < triangles.size(); i++){
            //triangles[i]->draw(window);
            triangles[i]->fill(window);
        }
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
