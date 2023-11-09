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

#define WIDTH 640
#define HEIGHT 480

void draw(DrawingWindow &window, DepthBuffer& depthBuffer, ModelLoader& model, Camera& camera, int frame) {
	window.clearPixels();
    depthBuffer.reset();
    vector<Triangle> tris = model.getTris();
    for(size_t i = 0; i < tris.size(); i++){
        Triangle thisTri = tris[tris.size() - i - 1]; //tested to see if rendering them in reverse order has any effect
        auto [pt0, valid0] = camera.getCanvasIntersectionPoint(thisTri.v0()); //project to flat (z becomes the distance to the camera)
        auto [pt1, valid1] = camera.getCanvasIntersectionPoint(thisTri.v1());
        auto [pt2, valid2] = camera.getCanvasIntersectionPoint(thisTri.v2());
        if(valid0 && valid1 && valid2){
            Colour thisColour = thisTri.getColour();
            thisTri.setV0(pt0);
            thisTri.setV1(pt1);
            thisTri.setV2(pt2);
            if(thisTri.isTextured()) {
                thisTri.fillTexture(window, depthBuffer);
            } else { thisTri.fill(window, depthBuffer); }
        }
    }

    camera.doOrbit(model);

}

void handleEvent(SDL_Event event, DrawingWindow &window, Camera& camera, ModelLoader& model) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_t) {
            camera.move(vec3(0.0, 0.0, -0.2));
        }
		else if (event.key.keysym.sym == SDLK_g) {
            camera.move(vec3(0.0, 0.0, 0.2));
        }
        if (event.key.keysym.sym == SDLK_f) {
            camera.move(vec3(-0.2, 0.0, 0.0));
        }
        else if (event.key.keysym.sym == SDLK_h) {
            camera.move(vec3(0.2, 0.0, 0.0));
        }
        if (event.key.keysym.sym == SDLK_r) {
            camera.move(vec3(0.0, -0.2, 0.0));
        }
        else if (event.key.keysym.sym == SDLK_y) {
            camera.move(vec3(0.0, 0.2, 0.0));
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

	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
    uint frame = 0;

    ModelLoader* cornellLoader = new ModelLoader("cornell-box.obj", 0.35, glm::vec3(0, 0, 0));
    cornellLoader->load();
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
		if (window.pollForInputEvents(event)) handleEvent(event, window, *camera, *cornellLoader);

        draw(window, *depthBuffer, *cornellLoader, *camera, frame);

        //camera->move(glm::vec3(0.0, -0.01, 0));
//        camera->lookAt(0.0, 0.0);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
        //if(frame % 6 == 0) std::cout << "frame" << frame << std::endl;
		window.renderFrame();
        frame = (frame + 1) % (SDL_MAX_UINT32);
	}
}
