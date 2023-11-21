//
// Created by elliot on 20/11/23.
//
#include "Camera.h"

#pragma once

/**
 * This class will render our scene for us, export the frames as a video
 */
class Cameraman {
public:
    Cameraman(Camera* cam, string outPath);

    void render(DrawingWindow& window, DepthBuffer& depthBuffer, ModelLoader& model, glm::vec4& light, bool withPreview);//renders all frames of the animation

    static Colour background;
    static SDL_Event event;
private:
    class Action { //private action class
    public:
        explicit Action(glm::mat3 args);
        //this act function modifies the camera and renders frames and saves them for a given action
        //returns the frames rendered
        virtual void act(DrawingWindow& window,
                         Camera& camera,
                         uint& frameID,
                         string& outPath,
                         ModelLoader& model,
                         DepthBuffer& depthBuffer,
                         glm::vec4 light,
                         bool withPreview) = 0; // =0 is a pure specifier (weird c++ stuff)
        void drawBackground(DrawingWindow& window);
    protected:
        glm::mat3 args;

    };

    class Lerp: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& outPath,
                 ModelLoader& model,
                 DepthBuffer& depthBuffer,
                 glm::vec4 light,
                 bool withPreview) override;
    };
    class Wait: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 ModelLoader& model,
                 DepthBuffer& depthBuffer,
                 glm::vec4 light,
                 bool withPreview) override;
    };
    class LerpRot: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 ModelLoader& model,
                 DepthBuffer& depthBuffer,
                 glm::vec4 light,
                 bool withPreview) override;
    };

    static const int FRAMERATE = 25; //fps (static)
    constexpr static const float STEP = static_cast<float>(1.0) / FRAMERATE; //evaulate at compile time

    std::vector<Action*> actions = {
            new Lerp(glm::mat3({0, -0.5, 4}, {1, -0.5, 4}, {0.5, 0, 0})),
            new Wait(glm::mat3({1, -0.5, 4}, {0, 0, 0}, {0.5, 0, 0})),
            new Lerp(glm::mat3({1, -0.5, 4}, {0, -0.5, 0.5}, {1, 0, 0})),
            new LerpRot(glm::mat3({0, 0, 0}, {M_PI / 8, -M_PI / 2, 0}, {1, 0, 0})),
            new LerpRot(glm::mat3({M_PI / 8, -M_PI / 2, 0}, {0, 0, 0}, {0.5, 0, 0})),
            new Lerp(glm::mat3({0, -0.5, 0.5}, {0, -0.5, 4}, {0.5, 0, 0})),
            new Wait(glm::mat3({0, -0.5, 4}, {0, 0, 0}, {0.75, 0, 0})),
            new Lerp(glm::mat3({0, -0.5, 4}, {0, -0.5, 0.5}, {0.15, 0, 0})),
            new LerpRot(glm::mat3({0, 2 * M_PI, 0}, {0, 0, 0}, {2.5, 0, 0})),
    };

    Camera* cam;
    string outPath;

};


