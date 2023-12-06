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
#include "Light.h"

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
    ModelLoader* blockBox =  new ModelLoader("tall_box.obj", 1, glm::vec3(4.5, 0, 0), 1.0, ModelLoader::nrm);
    ModelLoader* cornell = new ModelLoader("textured-cornell-box.obj", 0.35, glm::vec3(0, -0.5, 0), 0.75, ModelLoader::nrm);
    ModelLoader* metalSphere = new ModelLoader("sphere.obj", 0.35, glm::vec3(0.4, -0.25, -0.35), 0.5, ModelLoader::phg_mtl);
    ModelLoader* mirrorSphere = new ModelLoader("sphere.obj", 0.25, glm::vec3(-0.55, -0.2, 0.35), 0.0, ModelLoader::phg_mrr);
    ModelLoader* glassSphere = new ModelLoader("sphere.obj", 0.30, glm::vec3(-0.45, -1.1, 0.75), 0.2, ModelLoader::gls_phg);
    ModelLoader* gouraudSphere = new ModelLoader("sphere.obj", 0.35, glm::vec3(0.4, -0.25, -0.35), 1.0, ModelLoader::grd);;
    ModelLoader* tallBox = new ModelLoader("tall_box.obj", 0.25, glm::vec3(0.45, -1.0, 1), 0.3, ModelLoader::mtl);
    ModelLoader* tallBox2 = new ModelLoader("tall_box.obj", 1.0, glm::vec3(3.0, -1.0, 1), 0.4, ModelLoader::mtl);
    ModelLoader* glassBox = new ModelLoader("cube.obj", 0.25, glm::vec3(-0.45, -1.1, 0.75), 0.1, ModelLoader::gls, false);
    glassBox->rotate(M_PI_4, 0);
    ModelLoader* normalBox = new ModelLoader("../cube/cube.obj", 0.20, glm::vec3(-0.5, -0.6, 0.65), 0.5, ModelLoader::mtl, true, true);
    ModelLoader* hackspace = new ModelLoader("hackspace-logo/logo.obj", 0.002, glm::vec3(0.35, 0, 0.5), 1.0, ModelLoader::nrm);
    ModelLoader* mirrorBox = new ModelLoader("tall_box.obj", 0.4, glm::vec3(0.4, -1.2, -0.45), 0.0, ModelLoader::mrr);
    ModelLoader* glassBunny = new ModelLoader("LowpolyStanfordBunnyUprightEars.obj", 0.05, glm::vec3(0.0, 0.0, 0.0), 0.2, ModelLoader::gls_phg, false, false, true);
    mirrorBox->rotate(M_PI_4, 0);
    glassBunny->rotate(0, M_PI_2);

    std::vector<ModelLoader*> box = {cornell, blockBox};
    std::vector<ModelLoader*> boxGouraud = {cornell, gouraudSphere, blockBox};
    std::vector<ModelLoader*> boxMirrorMetal = {cornell, metalSphere, glassBox, tallBox, mirrorSphere, mirrorBox, blockBox};
    std::vector<ModelLoader*> boxHackspaceRefractPhongNormal = {cornell, metalSphere, mirrorSphere, hackspace, normalBox, glassSphere, gouraudSphere};
    std::vector<ModelLoader*> bunny = {glassBunny};

    DepthBuffer* depthBuffer = new DepthBuffer(WIDTH, HEIGHT);

    Light lightSoft = *(new Light(glm::mat3({-0.2, 0.35, 0}, {0.4, 0, 0}, {0, 0, 0.4}), glm::vec3({255, 255, 255}), 2, 1, 5 ));
    Light lightHard = *(new Light(glm::mat3({-0.2, 0.35, 0}, {0.01, 0, 0}, {0, 0, 0.01}), glm::vec3({255, 255, 255}), 2, 1, 2 ));
    Light lightUp = *(new Light(glm::mat3({-0.2, 1.35, 0}, {0.01, 0, 0}, {0, 0, 0.01}), glm::vec3({255, 255, 255}), 2, 1, 2 ));

    EnvMap envRocks = EnvMap("Ocean-from-horn.ppm");
    EnvMap envClouds = EnvMap("skybox.ppm");
    EnvMap envPark = EnvMap("skybox1.ppm");

    Scene* scene0 = new Scene(box, {lightHard}, envPark);
    Scene* scene1 = new Scene(boxGouraud, {lightHard}, envPark);
    Scene* scene2 = new Scene(boxMirrorMetal, {lightSoft}, envClouds);
    Scene* scene3 = new Scene(boxHackspaceRefractPhongNormal, {lightSoft}, envRocks);
    Scene* scene4 = new Scene(bunny, {lightUp}, envPark);

    Camera* camera = new Camera(glm::vec3(0.0, -0.5, 4.0), 2.0, glm::vec2(WIDTH, HEIGHT), scene0, 12);
    camera->setRot(0, 0);

    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	vector<vec3> resultVec = Utils::interpolateThreeElementValues(vec3(1.0, 4.0, 9.2), vec3(4.0, 1.0, 9.8), 4);
	for(int i=0; i < static_cast<int>(resultVec.size()); i++) {
		cout << "(";
		cout << resultVec[i].x << " ";
		cout << resultVec[i].y << " ";
		cout << resultVec[i].z << " ";
		cout << ")";
	}
	cout << endl;


//// //comment to stop the render
    std::vector<Scene*> scenes = {scene0, scene1, scene2, scene3, scene4};
    std::vector<float> sceneChanges = /*{0.1, 0.2, 0.3, 0.4};//*/{3.0, 5.0, 10.5, 15.0};
    Cameraman* cm = new Cameraman(camera,"./render/", scenes, sceneChanges);
    cm->render(window, *depthBuffer, true);

    while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window, *camera, *metalSphere);
        window.clearPixels();

        camera->doOrbit(*tallBox);
        camera->doRaytracing(window);
        camera->doRasterising(window, *depthBuffer);

        window.renderFrame();
        frame = (frame + 1) % (SDL_MAX_UINT32);
	}
}
