//
// Created by elliot on 20/11/23.
//

#include "Cameraman.h"

#include <utility>

void Cameraman::render(DrawingWindow& window, DepthBuffer& depthBuffer, bool withPreview) {
    uint frameID = 0;
    this->cam->setScene(this->scenes[this->currentScene]);
    for (Action *a: this->actions) {
        std::cout << "frame: " << frameID << std::endl;
        a->act(window,
               *this->cam,
               frameID,
               this->outPath,
               this->cam->getScene(),
               depthBuffer,
               withPreview);
        float time = static_cast<float>(frameID) / FRAMERATE; //scenes only change after actions
        if(time > this->sceneChanges[this->currentScene] && this->currentScene + 1 < this->scenes.size()){
            this->currentScene++;
            this->cam->setScene(this->scenes[this->currentScene]);
        }
    }
    std::cout << "rendered " + std::to_string(frameID) + " frames" << std::endl;
}

//std::vector<glm::vec3> Cameraman::Action::act();

Colour Cameraman::background = Colour(0, 0, 0);

SDL_Event Cameraman::event;

Cameraman::Action::Action(glm::mat3 args, std::vector<Ambient*> ambients) {
    this->args = args; //used differently depending on underlying implement
    this->ambients = std::move(ambients);
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

    std::cout << "Lerp" << std::endl;
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
        for(Ambient* a : ambients){
            a->act(window, camera, scene, depthBuffer);
        }
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

    std::cout << "Wait" << std::endl;
    glm::vec3* target = &this->args[0];
    float timeframe = this->args[2].x;
    if(timeframe <= 0){ throw runtime_error("Cameraman::Lerp::act: cannot perform an action for zero duration"); }
    int steps = static_cast<int>(glm::floor(Cameraman::FRAMERATE * timeframe));
    camera.setPos(*target);
    for(int i = 0; i < steps; i++){
        if (window.pollForInputEvents(event)){} //mandatory
        window.clearPixels();
        drawBackground(window);
        for(Ambient* a : ambients){
            a->act(window, camera, scene, depthBuffer);
        }
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

    std::cout << "LerpRot" << std::endl;
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
        for(Ambient* a : ambients){
            a->act(window, camera, scene, depthBuffer);
        }
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
    std::cout << "LerpModel" << std::endl;
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
        for(Ambient* a : ambients){
            a->act(window, camera, scene, depthBuffer);
        }
        scene.setModelPosition(modelIndex, pos);
        camera.doRaytracing(window);
        camera.doRasterising(window, depthBuffer);
        if(withPreview){ window.renderFrame(); }
        window.savePPM(out + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }
}

// row 1: start angle of cam x axis to world x, start angle of cam y axis to world y, IGNORED
// row 2:                   IGNORED
// row 3: timeframe,        model index,        == 1 ? from start angle : from current angle
void Cameraman::LookAtModel::act(DrawingWindow &window, Camera &camera, uint &frameID, string &out, Scene &scene,
                                 DepthBuffer &depthBuffer, bool withPreview) {
    std::cout << "LookAtModel" << std::endl;
    bool fromCurrent = this->args[2].z == 0;
    int modelIndex = static_cast<int>(glm::floor(this->args[2].y));
    glm::vec3 camRot = this->args[0];
    if(fromCurrent) camRot = glm::vec3(camera.getRot().x, camera.getRot().y, 0);
    camera.lookAt(*scene.getModel(modelIndex)->getPos()); //temporarily looks at model to find out rotation
    glm::vec3 camRotTo = glm::vec3(camera.getRot().x, camera.getRot().y, 0);
    Action* delegate = new LerpRot(glm::mat3({camRot, camRotTo, this->args[2]}), std::move(this->ambients));

    delegate->act(window, camera, frameID, out, scene, depthBuffer, withPreview);
}

Cameraman::Cameraman(Camera* cam, string outPath, std::vector<Scene*>& scenes, std::vector<float>& sceneChanges) {
    this->cam = cam;
    this->outPath = outPath;
    this->scenes = scenes;
    this->sceneChanges = sceneChanges;
    this->currentScene = 0;
}



    /**
     * run the following to compile the frames into a video at 25fps:
     *
     * ffmpeg -framerate 25 -i frame_%d.ppm -c:v libx264 -movflags +faststart output.mp4
     *
     *  **/

//lerps camera position and has the camera look towards a model (the lookat is instant, so write a LookAtModel instruction beforehand)
// row 1: currnt position x, currnt position y, currnt position z
// row 2: target position x, target position y, target position z
// row 3: timeframe x,       model index,         IGNORED z
void Cameraman::LerpLookat::act(DrawingWindow &window, Camera &camera, uint &frameID, string &out, Scene &scene,
                                DepthBuffer &depthBuffer, bool withPreview) {
    std::cout << "LerpLookat" << std::endl;
    int modelIndex = static_cast<int>(glm::floor(this->args[2].y));
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
        for(Ambient* a : ambients){
            a->act(window, camera, scene, depthBuffer);
        }
        camera.setPos(pos);
        camera.doRaytracing(window);
        camera.doRasterising(window, depthBuffer);
        camera.lookAt(*scene.getModel(modelIndex)->getPos());
        if(withPreview){ window.renderFrame(); }
        window.savePPM(out + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }
}


//instant action
// row 1:                 IGNORED
// row 2:                 IGNORED
// row 3: IGNORED x, Render mode enum y, IGNORED z
void Cameraman::SetMode::act(DrawingWindow &window, Camera &camera, uint &frameID, string &out, Scene &scene,
                             DepthBuffer &depthBuffer, bool withPreview) {
    std::cout << "SetMode" << std::endl;
    int mode = static_cast<int>(glm::floor(this->args[2].y));
    if(mode > Camera::ray || mode < Camera::msh){
        throw runtime_error("Cameraman::SetMode::act: action set up with an invalid render mode");
    }
    camera.setRenderMode(mode);
}

//orbit so many degrees around a model from the current position
// row 1:                          IGNORED
// row 2: angle (radians),         IGNORE y,            IGNORED z
// row 3: timeframe x,             model index,         IGNORED z
void Cameraman::Orbit::act(DrawingWindow &window, Camera &camera, uint &frameID, string &out, Scene &scene,
                           DepthBuffer &depthBuffer, bool withPreview) {
    float angle = this->args[1].x;
    float timeframe = this->args[2].x;
    int modelIndex = static_cast<int>(glm::floor(this->args[2].y));
    float angleStep = angle / glm::floor(timeframe *  Cameraman::FRAMERATE);
    int frames = glm::floor(timeframe * Cameraman::FRAMERATE);
    for(int i = 0; i < frames; i++){
        if (window.pollForInputEvents(event)){} //mandatory
        window.clearPixels();
        drawBackground(window);
        for(Ambient* a : ambients){
            a->act(window, camera, scene, depthBuffer);
        }
        camera.orbit(*scene.getModel(modelIndex), angleStep);
        camera.doRaytracing(window);
        camera.doRasterising(window, depthBuffer);
        if(withPreview){ window.renderFrame(); }
        window.savePPM(out + "frame_" + std::to_string(frameID) + ".ppm");
        frameID++;
    }
}

//effects that start with 'loop' are ambient and only happen during an action, action actually renders the frame
// row 1: yaw rate in rad/s,       pitch rate in rad/s, IGNORED
// row 2:                          IGNORED
// row 3: IGNORED,                 model index,         IGNORED z
void Cameraman::LoopRotModel::act(DrawingWindow &window, Camera &camera, Scene &scene, DepthBuffer &depthBuffer) {
    float yawRate = this->args[0].x;
    float pitchRate = this->args[0].y;
    int modelIndex = static_cast<int>(glm::floor(this->args[2].y));
    // one radian in one second is one radian in 25 frames
    scene.getModel(modelIndex)->rotate(yawRate / FRAMERATE, pitchRate / FRAMERATE);
}

Cameraman::Ambient::Ambient(glm::mat3 args) {
    this->args = args;
}
