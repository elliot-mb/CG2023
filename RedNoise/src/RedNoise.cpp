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
#include "ModelLoader.h"
#include "Camera.h"

using namespace std;
using namespace glm;

#define WIDTH 640
#define HEIGHT 480

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

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
    uint frame = 0;

    ModelLoader* cornellLoader = new ModelLoader("cornell-box.obj", 0.35);
    cornellLoader->load();
//    cornellLoader->printTris();

//    ModelLoader* icosahedron = new ModelLoader("icosahedron.obj", 1.0);
//    icosahedron->load();
//    //cornellLoader->printTris();
    DepthBuffer* depthBuffer = new DepthBuffer(WIDTH, HEIGHT);
    Camera* camera = new Camera(glm::vec3(0.0, 0, 4.0), 2.0, glm::vec2(WIDTH, HEIGHT));


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
        depthBuffer->reset();
        vector<ModelTriangle> tris = cornellLoader->getTris();
        for(size_t i = 0; i < tris.size(); i++){
            ModelTriangle thisTri = tris[tris.size() - i - 1]; //tested to see if rendering them in reverse order has any effect
            glm::vec3 pt0 = camera->getCanvasIntersectionPoint(thisTri.vertices[0]); //project to flat (z becomes the distance to the camera)
            glm::vec3 pt1 = camera->getCanvasIntersectionPoint(thisTri.vertices[1]);
            glm::vec3 pt2 = camera->getCanvasIntersectionPoint(thisTri.vertices[2]);
            Triangle t = *new Triangle(pt0, pt1, pt2, thisTri.colour);
            t.fill(window, *depthBuffer);
        }
        //camera->move(glm::vec3(0.0, -0.01, 0));
        camera->setRot(0.0, 0.0);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
        //if(frame % 6 == 0) std::cout << "frame" << frame << std::endl;
		window.renderFrame();
        frame = (frame + 1) % (SDL_MAX_UINT32);
	}
}
