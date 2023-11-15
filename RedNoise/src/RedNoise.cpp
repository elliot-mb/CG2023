#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include "Utils.h"
#include <vector>
#include <glm/glm.hpp>
#include <Colour.h>
#include <CanvasPoint.h>
#include "Triangle.h"
#include "ModelLoader.h"
#include "Camera.h"

using namespace std;
using namespace glm;

#define WIDTH 320
#define HEIGHT 240

void handleEvent(SDL_Event event, DrawingWindow &window, Camera& camera, ModelLoader& model) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_t) {
            camera.moveRelative(vec3(0.0, 0.0, -0.2));
        }
		else if (event.key.keysym.sym == SDLK_g) {
            camera.moveRelative(vec3(0.0, 0.0, 0.2));
        }
        if (event.key.keysym.sym == SDLK_f) {
            camera.moveRelative(vec3(-0.2, 0.0, 0.0));
        }
        else if (event.key.keysym.sym == SDLK_h) {
            camera.moveRelative(vec3(0.2, 0.0, 0.0));
        }
        if (event.key.keysym.sym == SDLK_r) {
            camera.moveRelative(vec3(0.0, -0.2, 0.0));
        }
        else if (event.key.keysym.sym == SDLK_y) {
            camera.moveRelative(vec3(0.0, 0.2, 0.0));
        }
        else if (event.key.keysym.sym == SDLK_4) {
            camera.rot(-0.2, 0);
        }
        else if (event.key.keysym.sym == SDLK_5) {
            camera.rot(0.2, 0);
        }
        else if (event.key.keysym.sym == SDLK_6) {
            camera.rot(0, -0.2);
        }
        else if (event.key.keysym.sym == SDLK_7) {
            camera.rot(0, 0.2);
        }
        else if (event.key.keysym.sym == SDLK_o) {
            camera.toggleOrbit();
        }
        else if (event.key.keysym.sym == SDLK_l) {
            camera.lookAt(model.getPos());
        }
        else if (event.key.keysym.sym == SDLK_SPACE){
            camera.renderMode();
        }
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
    uint frame = 0;

    ModelLoader* cornellLoader = new ModelLoader("sphere.obj", 0.35, glm::vec3(0, -0.5, 0));
    cornellLoader->load();
    DepthBuffer* depthBuffer = new DepthBuffer(WIDTH, HEIGHT);
    Camera* camera = new Camera(glm::vec3(0.0, 0, 4.0), 2.0, glm::vec2(WIDTH, HEIGHT));
    glm::vec4 light = glm::vec4(0.0, 1.5, 0.2, 1.5); //final is a strength

    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	vector<float> result = Utils::interpolateSingleFloats(2.2, 8.5, 7);
	for(int i=0; i < static_cast<int>(result.size()); i++) cout << result[i] << " ";
	cout << endl;

	vector<vec3> resultVec = Utils::interpolateThreeElementValues(vec3(1.0, 4.0, 9.2), vec3(4.0, 1.0, 9.8), 4);
	for(int i=0; i < static_cast<int>(resultVec.size()); i++) {
		cout << "(";
		cout << resultVec[i].x << " ";
		cout << resultVec[i].y << " ";
		cout << resultVec[i].z << " ";
		cout << ")";
	}
	cout << endl;



	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window, *camera, *cornellLoader);
        window.clearPixels();

        camera->doOrbit(*cornellLoader);
        camera->doRaytracing(window, *cornellLoader, light);
        camera->doRasterising(window, *cornellLoader, *depthBuffer);
        //draw(window, *depthBuffer, *cornellLoader, *camera, frame);

        //camera->move(glm::vec3(0.0, -0.01, 0));
//        camera->lookAt(0.0, 0.0);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
        //if(frame % 6 == 0) std::cout << "frame" << frame << std::endl;
		window.renderFrame();
        frame = (frame + 1) % (SDL_MAX_UINT32);
	}
}
