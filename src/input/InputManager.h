#pragma once

#include <functional>

struct GLFWwindow;

class InputManager {
public:
    using KeyHandler = std::function<void(int key, int action)>;
    using ResizeHandler = std::function<void(int width, int height)>;

    InputManager();
    explicit InputManager(GLFWwindow* window);

    void attach(GLFWwindow* window);
    void setKeyHandler(KeyHandler handler);
    void setResizeHandler(ResizeHandler handler);

private:
    GLFWwindow* window_;
    KeyHandler keyHandler_;
    ResizeHandler resizeHandler_;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
