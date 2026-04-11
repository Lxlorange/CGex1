#include "app/App.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <glad/glad.h>

#include "scene/Demo2D.h"
#include "scene/Demo3D.h"

App::App(GLFWwindow* window)
    : window_(window),
      inputManager_(window),
      demo2D_(std::make_unique<Demo2D>(PROJECT_SOURCE_DIR_PATH)),
      demo3D_(std::make_unique<Demo3D>(PROJECT_SOURCE_DIR_PATH)),
      activeScene_(nullptr),
      framebufferWidth_(800),
      framebufferHeight_(600),
      wireframe_(false) {
    if (window_ == nullptr) {
        throw std::runtime_error("App requires a valid GLFW window.");
    }

    glfwGetFramebufferSize(window_, &framebufferWidth_, &framebufferHeight_);
    glViewport(0, 0, framebufferWidth_, framebufferHeight_);
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwFocusWindow(window_);

    setupInput();
    switchToDemo2D();
}

App::~App() = default;

void App::run() {
    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window_)) {
        const float now = static_cast<float>(glfwGetTime());
        const float deltaTime = now - lastTime;
        lastTime = now;

        pollSceneKeys();
        renderOneFrame(deltaTime);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

bool App::runChecklistValidation(const std::string& screenshotDir) {
    namespace fs = std::filesystem;
    fs::create_directories(screenshotDir);

    auto shoot = [this, &screenshotDir](const std::string& fileName) {
        renderOneFrame(0.016f);
        captureFramePpm(screenshotDir + "/" + fileName);
        glfwSwapBuffers(window_);
        glfwPollEvents();
    };

    auto pressTimes = [this](int key, int times) {
        for (int i = 0; i < times; ++i) {
            pressKeyOnce(key);
        }
    };

    onKey(GLFW_KEY_1, GLFW_PRESS);
    shoot("checklist1_2d_draw.ppm");

    pressKeyOnce(GLFW_KEY_R);
    pressKeyOnce(GLFW_KEY_X);
    pressKeyOnce(GLFW_KEY_Q);
    pressKeyOnce(GLFW_KEY_D);
    pressKeyOnce(GLFW_KEY_H);
    pressKeyOnce(GLFW_KEY_Y);
    shoot("checklist2_2d_transform.ppm");

    pressKeyOnce(GLFW_KEY_R);
    pressKeyOnce(GLFW_KEY_D);
    pressKeyOnce(GLFW_KEY_Q);
    shoot("checklist3a_2d_combo_TR.ppm");
    pressKeyOnce(GLFW_KEY_T);
    shoot("checklist3b_2d_combo_RT.ppm");

    onKey(GLFW_KEY_2, GLFW_PRESS);
    shoot("checklist4_3d_draw.ppm");

    pressKeyOnce(GLFW_KEY_R);
    pressTimes(GLFW_KEY_UP, 12);
    pressTimes(GLFW_KEY_RIGHT, 10);
    pressTimes(GLFW_KEY_Z, 6);
    pressTimes(GLFW_KEY_I, 6);
    pressTimes(GLFW_KEY_O, 6);
    pressTimes(GLFW_KEY_EQUAL, 8);
    shoot("checklist5_3d_transform.ppm");

    pressKeyOnce(GLFW_KEY_R);
    pressTimes(GLFW_KEY_UP, 8);
    pressTimes(GLFW_KEY_LEFT, 6);
    pressTimes(GLFW_KEY_J, 6);
    pressTimes(GLFW_KEY_O, 4);
    pressTimes(GLFW_KEY_EQUAL, 4);
    shoot("checklist6a_3d_combo_TRS.ppm");
    pressKeyOnce(GLFW_KEY_T);
    shoot("checklist6b_3d_combo_RTS.ppm");

    pressKeyOnce(GLFW_KEY_R);
    pressTimes(GLFW_KEY_W, 4);
    pressTimes(GLFW_KEY_A, 4);
    pressTimes(GLFW_KEY_Q, 4);
    pressKeyOnce(GLFW_KEY_P);
    shoot("checklist7_camera_projection.ppm");

    return true;
}

void App::setupInput() {
    inputManager_.setKeyHandler([this](int key, int action) {
        onKey(key, action);
    });

    inputManager_.setResizeHandler([this](int width, int height) {
        onResize(width, height);
    });
}

void App::switchToDemo2D() {
    if (activeScene_ != nullptr) {
        activeScene_->onExit();
    }
    activeScene_ = demo2D_.get();
    activeScene_->onEnter();
    updateWindowTitle();
}

void App::switchToDemo3D() {
    if (activeScene_ != nullptr) {
        activeScene_->onExit();
    }
    activeScene_ = demo3D_.get();
    activeScene_->onEnter();
    updateWindowTitle();
}

void App::onKey(int key, int action) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        return;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        switchToDemo2D();
        return;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        switchToDemo3D();
        return;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        wireframe_ = !wireframe_;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe_ ? GL_LINE : GL_FILL);
        updateWindowTitle();
        return;
    }

    if (activeScene_ != nullptr && action == GLFW_PRESS) {
        activeScene_->onKey(key, action);
        updateWindowTitle();
    }
}

void App::onResize(int width, int height) {
    framebufferWidth_ = (width > 0) ? width : 1;
    framebufferHeight_ = (height > 0) ? height : 1;
    glViewport(0, 0, framebufferWidth_, framebufferHeight_);
}

void App::pollSceneKeys() {
    if (activeScene_ == nullptr) {
        return;
    }

    const int keys[] = {
        GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
        GLFW_KEY_Q, GLFW_KEY_E,
        GLFW_KEY_Z, GLFW_KEY_X,
        GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_N, GLFW_KEY_M,
        GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT,
        GLFW_KEY_I, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_O,
        GLFW_KEY_EQUAL, GLFW_KEY_MINUS, GLFW_KEY_KP_ADD, GLFW_KEY_KP_SUBTRACT
    };

    bool changed = false;
    for (int key : keys) {
        if (glfwGetKey(window_, key) == GLFW_PRESS) {
            activeScene_->onKey(key, GLFW_REPEAT);
            changed = true;
        }
    }

    if (changed) {
        updateWindowTitle();
    }
}

void App::updateWindowTitle() const {
    const bool is2D = (activeScene_ == demo2D_.get());
    const bool is3D = (activeScene_ == demo3D_.get());

    std::string title = "CG_ex1 | ";
    if (is2D) {
        title += "Demo2D";
    } else if (is3D) {
        title += "Demo3D";
    } else {
        title += "Unknown";
    }

    title += " | 1/2 switch | click to focus";
    title += wireframe_ ? " | Wireframe:ON" : " | Wireframe:OFF";

    if (is2D && demo2D_ != nullptr) {
        title += demo2D_->isAltOrder() ? " | Order:R*T*..." : " | Order:T*R*...";
    }

    if (is3D && demo3D_ != nullptr) {
        title += demo3D_->isPerspective() ? " | Proj:Perspective" : " | Proj:Ortho";
        title += demo3D_->isAltOrder() ? " | Order:R*T*S" : " | Order:T*R*S";

        const Vec3 eye = demo3D_->cameraEye();
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << " | Cam(" << eye.x << "," << eye.y << "," << eye.z << ")";
        title += ss.str();
    }

    glfwSetWindowTitle(window_, title.c_str());
}

void App::renderOneFrame(float deltaTime) {
    glClearColor(0.09f, 0.11f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (activeScene_ != nullptr) {
        activeScene_->update(deltaTime);
        activeScene_->render(framebufferWidth_, framebufferHeight_);
    }
}

void App::captureFramePpm(const std::string& imagePath) const {
    const int w = framebufferWidth_;
    const int h = framebufferHeight_;
    std::vector<unsigned char> pixels(static_cast<size_t>(w) * static_cast<size_t>(h) * 3);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    std::ofstream out(imagePath, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to write screenshot: " + imagePath);
    }

    out << "P6\n" << w << " " << h << "\n255\n";
    for (int row = h - 1; row >= 0; --row) {
        const unsigned char* line = pixels.data() + static_cast<size_t>(row) * static_cast<size_t>(w) * 3;
        out.write(reinterpret_cast<const char*>(line), static_cast<std::streamsize>(w * 3));
    }
}

void App::pressKeyOnce(int key) {
    onKey(key, GLFW_PRESS);
}
