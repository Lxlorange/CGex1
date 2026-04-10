#pragma once

#include <memory>
#include <string>

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include "input/InputManager.h"
#include "scene/Scene.h"

class Demo2D;
class Demo3D;

class App {
public:
    explicit App(GLFWwindow* window);
    ~App();

    void run();
    bool runChecklistValidation(const std::string& screenshotDir);

private:
    GLFWwindow* window_;
    InputManager inputManager_;

    std::unique_ptr<Demo2D> demo2D_;
    std::unique_ptr<Demo3D> demo3D_;
    Scene* activeScene_;

    int framebufferWidth_;
    int framebufferHeight_;
    bool wireframe_;

    void setupInput();
    void switchToDemo2D();
    void switchToDemo3D();
    void onKey(int key, int action);
    void onResize(int width, int height);
    void pollSceneKeys();
    void updateWindowTitle() const;
    void renderOneFrame(float deltaTime);
    void captureFramePpm(const std::string& imagePath) const;
    void pressKeyOnce(int key);
};
