//
// Created by elliot on 20/11/23.
//

#include "Cameraman.h"

//std::vector<glm::vec3> Cameraman::Action::act();

Colour Cameraman::background = Colour(0, 0, 0);
SDL_Event Cameraman::event;

Cameraman::Action::Action(glm::mat3 args) {
    this->args = args; //used differently depending on underlying implement
}

void Cameraman::Action::drawBackground(DrawingWindow &window) {
    for(size_t y = 0; y < window.height; y++){
        for(size_t x = 0; x < window.width; x++){
            window.setPixelColour(x, y, Utils::pack(255, Cameraman::background.red, Cameraman::background.green, Cameraman::background.blue));
        }
    }
}

// mat3
// row 1: currnt position x, currnt position y, currnt position z
// row 2: target position x, target position y, target position z
// row 3: timeframe x,       IGNORED y,         IGNORED z
void Cameraman::Lerp::act(DrawingWindow& window,
                          Camera& camera,
                          uint& frameID,
                          string& out,
                          Scene &scene,
                          DepthBuffer& depthBuffer,
                          bool withPreview) {

    std::cout << "lerp" << std::endl;
    glm::vec3* current = &this->args[0];
    glm::vec3* target = &this->args[1];
    float timeframe = this->args[2].x;
    if(timeframe <= 0){ throw runtime_error("Cameraman::Lerp::act: cannot perform an action for zero duration"); }
    int steps = static_cast<int>(glm::floor(Cameraman::FRAMERATE * timeframe));
    std::vector<glm::vec3> frames =  Utils::interpolateThreeElementValues(*current, *target, steps);
    for(glm::vec3 pos : frames){
        if (window.pollForInputEvents(event)){} //mandatory
        window.clearPixels();
        drawBackground(window);

        camera.setPos(pos);
        camera.doRaytracing(window);
        camera.doRasterising(window, depthBuffer);
        if(withPreview){ window.renderFrame(); }
        window.savePPM(out + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }
}

// mat3
// row 1: target position x, target position y, target position z
// row 2:                    IGNORED
// row 3: timeframe x,       IGNORED y,         IGNORED z
void Cameraman::Wait::act(DrawingWindow& window,
                          Camera& camera,
                          uint& frameID,
                          string& out,
                          Scene &scene,
                          DepthBuffer& depthBuffer,
                          bool withPreview) {

    std::cout << "wait" << std::endl;
    glm::vec3* target = &this->args[0];
    float timeframe = this->args[2].x;
    if(timeframe <= 0){ throw runtime_error("Cameraman::Lerp::act: cannot perform an action for zero duration"); }
    int steps = static_cast<int>(glm::floor(Cameraman::FRAMERATE * timeframe));
    camera.setPos(*target);
    for(int i = 0; i < steps; i++){
        if (window.pollForInputEvents(event)){} //mandatory
        window.clearPixels();
        drawBackground(window);
        camera.doRaytracing(window);
        camera.doRasterising(window, depthBuffer);
        if(withPreview){ window.renderFrame(); }
        window.savePPM(out + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }
}

// mat3
// row 1: current angle of cam x axis to world x, current angle of cam y axis to world y, IGNORED
// row 2: target angle of cam x axis to world x,  target angle of cam y axis to world y,  IGNORED
// row 3: timeframe x,                        IGNORED,                      IGNORED
void Cameraman::LerpRot::act(DrawingWindow& window,
                             Camera& camera,
                             uint& frameID,
                             string& out,
                             Scene &scene,
                             DepthBuffer& depthBuffer,
                             bool withPreview) {

    std::cout << "rot" << std::endl;
    float* xCurrent = &this->args[0].x;
    float* yCurrent = &this->args[0].y;
    float* xTarget = &this->args[1].x;
    float* yTarget = &this->args[1].y;
    float* timeframe = &this->args[2].x;
    if(*timeframe <= 0){ throw runtime_error("Cameraman::Lerp::act: cannot perform an action for zero duration"); }
    int steps = static_cast<int>(glm::floor(Cameraman::FRAMERATE * *timeframe));
    std::vector<glm::vec2> frames = Utils::interpolateTwoElementValues(glm::vec2(*xCurrent, *yCurrent), glm::vec2(*xTarget, *yTarget), steps);
    for(glm::vec2 angles : frames){
        if (window.pollForInputEvents(event)){} //mandatory
        window.clearPixels();
        drawBackground(window);

        camera.setRot(angles.x, angles.y);
        camera.doRaytracing(window);
        camera.doRasterising(window, depthBuffer);
        if(withPreview){ window.renderFrame(); }
        window.savePPM(out + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }

}

// row 1: start position x, start position y, start position z
// row 2: end position x,   end position y,   end position z
// row 3: timeframe,        model index,      IGNORED
void Cameraman::LerpModel::act(DrawingWindow &window,
                               Camera &camera,
                               uint &frameID,
                               string &out,
                               Scene &scene,
                               DepthBuffer &depthBuffer,
                               bool withPreview) {
    std::cout << "lerp model" << std::endl;
    glm::vec3 *start = &this->args[0];
    glm::vec3 *end = &this->args[1];
    float timeframe = this->args[2].x;
    int modelIndex = static_cast<int>(glm::floor(this->args[2].y));
    if(this->args[2].y - static_cast<float>(modelIndex) != 0){ throw runtime_error("Cameraman::LerpModel::act: cannot lerp model at non integer index"); }
    int steps = static_cast<int>(glm::floor(Cameraman::FRAMERATE * timeframe));
    std::vector<glm::vec3> frames =  Utils::interpolateThreeElementValues(*start, *end, steps);
    for(glm::vec3 pos : frames){
        if (window.pollForInputEvents(event)){} //mandatory
        window.clearPixels();
        drawBackground(window);

        scene.setModelPosition(modelIndex, pos);

        camera.doRaytracing(window);
        camera.doRasterising(window, depthBuffer);
        if(withPreview){ window.renderFrame(); }
        window.savePPM(out + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }
}

// row 1: current angle of cam x axis to world x, current angle of cam y axis to world y, IGNORED
// row 2:                   IGNORED
// row 3: timeframe,        model index,          IGNORED
void Cameraman::LookAtModel::act(DrawingWindow &window, Camera &camera, uint &frameID, string &out, Scene &scene,
                                 DepthBuffer &depthBuffer, bool withPreview) {
    int modelIndex = static_cast<int>(glm::floor(this->args[2].y));
    std::cout << modelIndex << std::endl;
    camera.lookAt(*scene.getModel(modelIndex)->getPos()); //temporarily looks at model to find out rotation
    glm::vec2 camRot = camera.getRot();
    std::cout << camRot.x << "," << camRot.y << std::endl;
    Action* delegate = new LerpRot(glm::mat3({this->args[0], glm::vec3(camRot, 0), this->args[2]}));

    delegate->act(window, camera, frameID, out, scene, depthBuffer, withPreview);
}

Cameraman::Cameraman(Camera* cam, string outPath) {
    this->cam = cam;
    this->outPath = outPath;
}

void Cameraman::render(DrawingWindow& window, DepthBuffer& depthBuffer, Scene& scene, glm::vec4& light, bool withPreview) {
    uint frameID = 0;

    for(Action* a : this->actions){
        a->act(window,
              *this->cam,
              frameID,
              this->outPath,
              scene,
              depthBuffer,
              withPreview);
    }
    std::cout << "rendered " + std::to_string(frameID) + " frames" << std::endl;

    /**
     * run the following to compile the frames into a video at 25fps:
     *
     * ffmpeg -framerate 25 -i frame_%d.ppm -c:v libx264 -movflags +faststart output.mp4
     *
     *         for(glm::vec3 pos : positionBuffer){
            if (window.pollForInputEvents(event)){} //mandatory
            window.clearPixels();
            drawBackground(window);

            this->cam->setPos(pos);
            this->cam->doRaytracing(window, model, light);
            this->cam->doRasterising(window, model, depthBuffer);
            if(withPreview){ window.renderFrame(); }
            window.savePPM(this->outPath + "frame_" + std::to_string(frameID) + ".ppm");
            frameID++;
        }
     */

}

