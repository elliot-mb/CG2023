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
#include "Cameraman.h"
#include "Scene.h"

using namespace std;
using namespace glm;

#define WIDTH 640
#define HEIGHT 480

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
            camera.lookAt(*model.getPos());
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

    ModelLoader* cornell = new ModelLoader("textured-cornell-box.obj", 0.35, glm::vec3(0, -0.5, 0), ModelLoader::nrm);
    ModelLoader* sphere = new ModelLoader("sphere.obj", 0.35, glm::vec3(0.4, -0.25, -0.35), 0.5, 0.0, true, WIDTH, HEIGHT);
    ModelLoader* sphere2 = new ModelLoader("sphere.obj", 0.20, glm::vec3(-0.64, -0.25, 0.75), ModelLoader::phg);
    ModelLoader* tallBox = new ModelLoader("tall_box.obj", 0.25, glm::vec3(0.45, -1.0, 1), 1.0, 0.0, false, WIDTH, HEIGHT);
    ModelLoader* mirrorBox = new ModelLoader("tall_box.obj", 0.25, glm::vec3(-0.6, -1.0, 1), ModelLoader::mrr);
    std::vector<ModelLoader*> models = { cornell, sphere, tallBox, mirrorBox, sphere2 };

    DepthBuffer* depthBuffer = new DepthBuffer(WIDTH, HEIGHT);

    glm::vec4 light = glm::vec4(0.0, 0.25,  0.0, 0.5); //final is a strength
    glm::vec4 light2 = glm::vec4(0.15, 0.25,  0.0, 0.5); //so good they made a second one
    glm::vec4 light3 = glm::vec4(-0.15, 0.25,  0.0, 0.5); //so good they made a second second one
    glm::vec4 light4 = glm::vec4(0.1, 0.25,  0.0, 0.5); //so good they made a second one
    glm::vec4 light5 = glm::vec4(-0.1, 0.25,  0.0, 0.5); //so good they made a second second one

    Scene* s = new Scene(models, {&light, &light2, &light3, &light4, &light5});
    Camera* camera = new Camera(glm::vec3(0.0, 0, 4.0), 2.0, glm::vec2(WIDTH, HEIGHT), s, 7);

    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

//
    sphere->blurFuzzMap();
    sphere->blurFuzzMap();

	vector<vec3> resultVec = Utils::interpolateThreeElementValues(vec3(1.0, 4.0, 9.2), vec3(4.0, 1.0, 9.8), 4);
	for(int i=0; i < static_cast<int>(resultVec.size()); i++) {
		cout << "(";
		cout << resultVec[i].x << " ";
		cout << resultVec[i].y << " ";
		cout << resultVec[i].z << " ";
		cout << ")";
	}
	cout << endl;


 //comment to stop the render
//    Cameraman* cm = new Cameraman(camera, "./render/");
//    cm->render(window, *depthBuffer, *s, light, true);

    while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window, *camera, *sphere);
        window.clearPixels();

        camera->doOrbit(*tallBox);
        camera->doRaytracing(window);
        camera->doRasterising(window, *depthBuffer);

//        light += glm::vec4(0.0, glm::cos(frame * 0.2) * 0.1, glm::sin(frame * 0.2) * 0.1, 0.0);
//        sphere->setPos(glm::vec3(0, sphere->getPos()->y + 0.01, 0));

        window.renderFrame();
        frame = (frame + 1) % (SDL_MAX_UINT32);
	}
}
