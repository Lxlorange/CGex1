#include "input/InputManager.h"

#include <utility>

#include <GLFW/glfw3.h>

InputManager::InputManager()
    : window_(nullptr) {}

InputManager::InputManager(GLFWwindow* window)
    : window_(nullptr) {
    attach(window);
}

void InputManager::attach(GLFWwindow* window) {
    window_ = window;
    if (window_ == nullptr) {
        return;
    }

    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, InputManager::keyCallback);
    glfwSetFramebufferSizeCallback(window_, InputManager::framebufferSizeCallback);
}

void InputManager::setKeyHandler(KeyHandler handler) {
    keyHandler_ = std::move(handler);
}

void InputManager::setResizeHandler(ResizeHandler handler) {
    resizeHandler_ = std::move(handler);
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    InputManager* input = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (input == nullptr || !input->keyHandler_) {
        return;
    }

    input->keyHandler_(key, action);
}

void InputManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    InputManager* input = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (input != nullptr && input->resizeHandler_) {
        input->resizeHandler_(width, height);
    }
}
