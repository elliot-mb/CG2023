//
// Created by elliot on 20/11/23.
//

#include "Cameraman.h"

//std::vector<glm::vec3> Cameraman::Action::act();

Cameraman::Action::Action(glm::mat3 args) {
    this->args = args; //used differently depending on underlying implement
}

// mat3
// row 1: currnt position x, currnt position y, currnt position z
// row 2: target position x, target position y, target position z
// row 3: timeframe x,       IGNORED y,         IGNORED z
uint Cameraman::Lerp::act(DrawingWindow& window, Camera& camera, uint frameID, string& outFile) {
    std::cout << "lerp" << std::endl;
    glm::vec3* current = &this->args[0];
    glm::vec3* target = &this->args[1];
    float timeframe = this->args[2].x;
    if(timeframe <= 0){ throw runtime_error("Cameraman::Lerp::act: cannot perform an action for zero duration"); }
    int steps = static_cast<int>(glm::floor(Cameraman::FRAMERATE * timeframe));
    return Utils::interpolateThreeElementValues(*current, *target, steps);
}

// mat3
// row 1: target position x, target position y, target position z
// row 2: timeframe x,       IGNORED y,         IGNORED z
// row 3:                    IGNORED
uint Cameraman::Wait::act(DrawingWindow& window, Camera& camera, uint frameID, string& outFile) {
    std::cout << "wait" << std::endl;
    glm::vec3* target = &this->args[0];
    float timeframe = this->args[1].x;
    if(timeframe <= 0){ throw runtime_error("Cameraman::Lerp::act: cannot perform an action for zero duration"); }
    int steps = static_cast<int>(glm::floor(Cameraman::FRAMERATE * timeframe));
    std::vector<glm::vec3> frames = Utils::interpolateThreeElementValues(*target, *target, steps); // essentially just how many frames we need to say in the same place
    SDL_Event event;
    for(glm::vec3 pos : frames){
        if (window.pollForInputEvents(event)){} //mandatory
        window.clearPixels();
        drawBackground(window);

        camera.setPos(pos);
        camera.doRaytracing(window, model, light);
        camera.doRasterising(window, model, depthBuffer);
        if(withPreview){ window.renderFrame(); }
        window.savePPM(this->outPath + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }
}

// mat3
// row 1: current angle of cam x axis to world x, current angle of cam y axis to world y, IGNORED
// row 2: target angle of cam x axis to world x,  target angle of cam y axis to world y,  IGNORED
// row 3: timeframe x,                        IGNORED,                      IGNORED
uint Cameraman::LerpRot::act(DrawingWindow& window, Camera& camera, uint frameID, string& outFile) {
    return std::vector<glm::vec3>();
}

Cameraman::Cameraman(Camera* cam, string outPath, Colour background) {
    this->cam = cam;
    this->outPath = outPath;
    this->background = background;
}

void Cameraman::render(DrawingWindow& window, DepthBuffer& depthBuffer, ModelLoader& model, glm::vec4& light, bool withPreview) {
    uint frameID = 0;

    for(Action* a : this->actions){
        frameID += a->act(window, *this->cam);
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

void Cameraman::drawBackground(DrawingWindow &window) {
    for(size_t y = 0; y < window.height; y++){
        for(size_t x = 0; x < window.width; x++){
            window.setPixelColour(x, y, Utils::pack(255, this->background.red, this->background.green, this->background.blue));
        }
    }
}
