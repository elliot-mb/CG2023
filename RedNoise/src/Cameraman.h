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


    Cameraman(Camera* cam, string outPath, std::vector<Scene>& scenes, std::vector<float>& sceneChanges);

    void render(DrawingWindow& window, DepthBuffer& depthBuffer, Scene& scene, bool withPreview);//renders all frames of the animation

    static Colour background;
    static SDL_Event event;
private:

    class Ambient {
    public:
        explicit Ambient(glm::mat3 args);
        virtual void act(DrawingWindow& window,
                         Camera& camera,
                         Scene &scene,
                         DepthBuffer& depthBuffer) = 0;
    protected:
        glm::mat3 args;
    };
    class LoopRotModel: public Ambient{
        using Ambient::Ambient;
        void act(DrawingWindow& window,
                 Camera& camera,
                 Scene &scene,
                 DepthBuffer& depthBuffer) override;
    };

    class Action { //private action class
    public:
        Action(glm::mat3 args, std::vector<Ambient*> ambients);
        //this act function modifies the camera and renders frames and saves them for a given action
        //returns the frames rendered
        virtual void act(DrawingWindow& window,
                         Camera& camera,
                         uint& frameID,
                         string& outPath,
                         Scene &scene,
                         DepthBuffer& depthBuffer,
                         bool withPreview) = 0; // =0 is a pure specifier (weird c++ stuff)
        void drawBackground(DrawingWindow& window);
        static std::vector<Action*> ambientActions; //actions for all classes

    protected:
        glm::mat3 args;
        std::vector<Ambient*> ambients;
    };

    class Lerp: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& outPath,
                 Scene &scene,
                 DepthBuffer& depthBuffer,
                 bool withPreview) override;
    };
    class Wait: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 Scene &scene,
                 DepthBuffer& depthBuffer,
                 bool withPreview) override;
    };
    class LerpRot: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 Scene &scene,
                 DepthBuffer& depthBuffer,
                 bool withPreview) override;
    };
    class LerpModel: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 Scene &scene,
                 DepthBuffer& depthBuffer,
                 bool withPreview) override;
    };
    class LookAtModel: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 Scene &scene,
                 DepthBuffer& depthBuffer,
                 bool withPreview) override;
    };
    class LerpLookat: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 Scene &scene,
                 DepthBuffer& depthBuffer,
                 bool withPreview) override;
    };
    class SetMode: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                  Camera& camera,
                  uint& frameID,
                  string& out,
                  Scene &scene,
                  DepthBuffer& depthBuffer,
                  bool withPreview) override;
    };
    class Orbit: public Action{
        using Action::Action;
        void act(DrawingWindow& window,
                 Camera& camera,
                 uint& frameID,
                 string& out,
                 Scene &scene,
                 DepthBuffer& depthBuffer,
                 bool withPreview) override;
    };

    std::vector<Scene> scenes; //scenes to switch over
    std::vector<float> sceneChanges; // in seconds
    int currentScene;

    static const int FRAMERATE = 25; //fps (static)
    constexpr static const float STEP = static_cast<float>(1.0) / FRAMERATE; //evaulate at compile time
    //blank aciton: {{0,0,0}, {0,0,0}, {1,0,0}}, {}
    std::vector<Action*> actions = {
            new SetMode({{0,0,0}, {0,0,0}, {0,Camera::msh,0}}, {}),
            new Wait({{0,-0.5,4}, {0,0,0}, {0.5,0,0}}, {}),
            new Lerp({{0,-0.5,4}, {0,-0.5,6}, {0.5,0,0}}, {}),
            new Orbit({{0,0,0}, {3*M_PI/2,0,0}, {0.75,0,0}}, {}),
            new SetMode({{0,0,0}, {0,0,0}, {0,Camera::rst,0}}, {}),
            new Orbit({{0,0,0}, {M_PI/2,0,0}, {0.25,0,0}}, {}),
            new Lerp({{0,-0.5,6}, {0,-0.5,4}, {0.25,0,0}}, {}),
            new Lerp({{0,-0.5,4}, {0,-0.5,6}, {0.25,0,0}}, {}),
            new Orbit({{0,0,0}, {3*M_PI/2,0,0}, {0.75,0,0}}, {}),
            new SetMode({{0,0,0}, {0,0,0}, {0,Camera::ray,0}}, {}),
            new Orbit({{0,0,0}, {M_PI/2,0,0}, {0.25,0,0}}, {}),




















            //new Lerp(glm::mat3({0, 2, 4}, {0, -1, 3.5}, {1.5, 0, 0})),
            //new Wait(glm::mat3({0, -1, 3.5}, {0, 0, 0}, {0.5, 0, 0})),
//            new SetMode(glm::mat3({0, 0, 0}, {0, 0, 0}, {0, 0, 0})),
//            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 0, 0})),
//            new Wait(glm::mat3({0.0, 0, 4.0}, {0, 0, 0}, {1.0, 0, 0})),
//            new SetMode(glm::mat3({0, 0, 0}, {0, 0, 0}, {0, 1, 0})),
//            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 1, 0})),
//            new Wait(glm::mat3({0.0, 0, 4.0}, {0, 0, 0}, {1.0, 0, 0})),
//            new SetMode(glm::mat3({0, 0, 0}, {0, 0, 0}, {0, 2, 0})),
//            new Wait(glm::mat3({0.0, 0, 4.0}, {0, 0, 0}, {1.0, 0, 0})),
//            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 1, 0})),
//            new LerpLookat(glm::mat3({0, 0, 4.0}, {0.5, 1, 4}, {2, 1, 0})),
//            new Wait(glm::mat3({0.5, 1, 4}, {0, 0, 0}, {1.0, 0, 0})),
//            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 2, 0})),



/*            new SetMode(glm::mat3({0, 0, 0}, {0, 0, 0}, {0, 1, 0}), {}),
            new Wait(glm::mat3({0, -0.5, 4}, {0, 0, 0}, {2, 0, 0}), {new LoopRotModel(glm::mat3({{6, 0, 0}, {0, 0, 0}, {0, 0, 0}}))}),
            new Orbit(glm::mat3({0, 0, 0}, {-M_PI, 0, 0}, {2, 0, 0}), {})*/

            /*new SetMode(glm::mat3({0, 0, 0}, {0, 0, 0}, {0, 0, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 0, 0})),
            new SetMode(glm::mat3({0, 0, 0}, {0, 0, 0}, {0, 1, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 1, 0})),
            new LerpLookat(glm::mat3({0, 0, 4}, {0, 0.25, 2}, {1, 1, 0})),
            new SetMode(glm::mat3({0, 0, 0}, {0, 0, 0}, {0, 2, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 0, 0})),
            new LerpLookat(glm::mat3({0, 0, 2}, {0, 0, 4}, {1, 0, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 1, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 2, 0})),
            new LerpLookat(glm::mat3({0, 0, 4}, {-0.25, 0, -0.7}, {2, 2, 0})),
//            new Wait(glm::mat3({-0.25, 0, -0.7}, {0, 0, 0}, {1.0, 0, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 1, 0})),
            new LerpLookat(glm::mat3({-0.25, 0, -0.7}, {-0.25, 0, -0.25}, {1, 1, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.5, 4, 0})),
            new LerpLookat(glm::mat3({-0.25, 0, -0.25}, {-0.5, 0, 2}, {2, 4, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {1.5, 5, 0})),
            new LerpLookat(glm::mat3({-0.5, 0, 2}, {0, 0, 3}, {2, 5, 0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {0.75, 0, 0})),
            new LerpLookat(glm::mat3({0, 0, 3}, {0, 0, 4}, {2, 0, 0})),
            new LerpModel(glm::mat3({-0.64, -0.25, 0.75}, {0.64, -0.25, 0.75}, {2.0, 4, 0.0})),
            new LerpModel(glm::mat3({-0.45, -0.75, 1}, {1, 1, 3}, {2.0, 3, 0.0})),
            new LookAtModel(glm::mat3({0, 0, 0}, {0, 0, 0}, {1.5, 3, 0})),*/


//            new LerpModel(glm::mat3({-0.25, -0.90, 0}, {0.7, -0.35, 0}, {1.0, 1.0, 0.0})),
//            new Lerp(glm::mat3({0, -1.2, 3.5}, {-0.25, -0.5, 0.0}, {1.5, 0, 0})),
//            new LerpRot(glm::mat3({0, 0, 0}, {-M_PI / 6, M_PI / 3, 0}, {1, 0, 0})),
//            new Wait(glm::mat3({-0.25, -0.5, 0.0}, {0, 0, 0}, {1.0, 0, 0})),
//            new Lerp(glm::mat3({-0.25, -0.5, 0.0}, {-0.25, -0.5, 0.5}, {1.25, 0, 0})),
//            new LerpRot(glm::mat3({-M_PI / 6, M_PI / 3, 0}, {0, 0, 0}, {1, 0, 0})),
//            new Lerp(glm::mat3({{-0.25, -0.5, 0.5}, {0, -1.2, 3.5}, {1.5, 0, 0}})),
//            new Lerp(glm::mat3({{0, -1.2, 3.5}, {0, -3, 3.5}, {0.75, 0, 0}})),
//            new Wait(glm::mat3({0, 0, 4}, {0, 0, 0}, {0.25, 0, 0})),
//            new LerpModel(glm::mat3({0, 0, 0}, {0, 2.0, 0}, {1.0, 1.0, 0.0})),
//            new Lerp(glm::mat3({0, 0, 4}, {0, 0.35, 2}, {0.5, 0, 0})),
//            new Wait(glm::mat3({0, 0.5, 2}, {0, 0, 0}, {0.25, 0, 0})),
//            new LerpRot(glm::mat3({0, 0, 0}, {M_PI / 5, 0, 0}, {1, 0, 0})),
//            new LerpRot(glm::mat3({0, 0, 0}, {- M_PI / 2.5, 0, 0}, {2, 0, 0})),
//            new Wait(glm::mat3({0, 0.5, 2}, {0, 0, 0}, {1.5, 0, 0})),
//            new Lerp(glm::mat3({0, 0.5, 2}, {0, -0.5, 4}, {0.5, 0, 0})),
//            new Lerp(glm::mat3({1, -0.5, 4}, {0, -0.5, 0.5}, {1, 0, 0})),
//            new LerpRot(glm::mat3({0, 0, 0}, {M_PI / 8, -M_PI / 2, 0}, {1, 0, 0})),
//            new LerpRot(glm::mat3({M_PI / 8, -M_PI / 2, 0}, {0, 0, 0}, {0.5, 0, 0})),
//            new Lerp(glm::mat3({0, -0.5, 0.5}, {0, -0.5, 4}, {0.5, 0, 0})),
//            new Wait(glm::mat3({0, -0.5, 4}, {0, 0, 0}, {0.75, 0, 0})),
//            new Lerp(glm::mat3({0, -0.5, 4}, {0, -0.5, 0.5}, {0.15, 0, 0})),
//            new LerpRot(glm::mat3({0, 2 * M_PI, 0}, {0, 0, 0}, {2.5, 0, 0})),
    };

    Camera* cam;
    string outPath;

};


