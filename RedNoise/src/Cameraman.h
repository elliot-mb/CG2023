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
    Cameraman(Camera* cam, string outPath, Colour background);

    void render(DrawingWindow& window, DepthBuffer& depthBuffer, ModelLoader& model, glm::vec4& light, bool withPreview);//renders all frames of the animation


private:
    class Action { //private action class
    public:
        explicit Action(glm::mat3 args);
        virtual std::vector<glm::vec3> act() = 0; // =0 is a pure specifier (weird c++ stuff)
    protected:
        glm::mat3 args;
    };

    class Lerp: public Action{
        using Action::Action;
        std::vector<glm::vec3> act() override;
    };
    class Wait: public Action{
        using Action::Action;
        std::vector<glm::vec3> act() override;
    };

    static const int FRAMERATE = 25; //fps (static)
    constexpr static const float STEP = static_cast<float>(1.0) / FRAMERATE; //evaulate at compile time

    std::vector<Action*> actions = {
            new Lerp(glm::mat3({0, -0.5, 4}, {1, -0.5, 4}, {1, 0, 0})),
            new Wait(glm::mat3({1, -0.5, 4}, {1, 0, 0}, {0, 0, 0})),
            new Lerp(glm::mat3({1, -0.5, 4}, {0, -0.5, 2}, {1, 0, 0})),
    };

    Camera* cam;
    string outPath;
    Colour background;

    void drawBackground(DrawingWindow& window);
};


