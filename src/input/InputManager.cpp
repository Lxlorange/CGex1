#include "input/InputManager.h"

#include <utility>

#include <imgui.h>
#include <imgui_impl_glfw.h>
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
    glfwSetMouseButtonCallback(window_, InputManager::mouseButtonCallback);
    glfwSetScrollCallback(window_, InputManager::scrollCallback);
    glfwSetDropCallback(window_, InputManager::dropCallback);
}

void InputManager::setKeyHandler(KeyHandler handler) {
    keyHandler_ = std::move(handler);
}

void InputManager::setResizeHandler(ResizeHandler handler) {
    resizeHandler_ = std::move(handler);
}

void InputManager::setDropHandler(DropHandler handler) {
    dropHandler_ = std::move(handler);
}

void InputManager::dropCallback(GLFWwindow* window, int count, const char** paths) {
    InputManager* input = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (input != nullptr && input->dropHandler_ && count > 0) {
        // 直接取第一个文件的路径传给 App
        input->dropHandler_(paths[0]);
    }
}

void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        if (ImGui::GetIO().WantCaptureKeyboard) {
            return;
        }
    }

    InputManager* input = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (input == nullptr || !input->keyHandler_) {
        return;
    }

    input->keyHandler_(key, action);
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    }
}

void InputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    }
}

void InputManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    InputManager* input = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (input != nullptr && input->resizeHandler_) {
        input->resizeHandler_(width, height);
    }
}
