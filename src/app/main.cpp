#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"

namespace {
void printHelp() {
    std::cout << "CG_ex1 usage:\n";
    std::cout << "  CG_ex1.exe              Run interactive demo\n";
    std::cout << "  CG_ex1.exe --validate   Auto-run checklist and dump screenshots\n";
    std::cout << "  CG_ex1.exe --help       Show this help\n\n";

    std::cout << "Global keys:\n";
    std::cout << "  1 / 2  Switch Demo2D / Demo3D\n";
    std::cout << "  F      Toggle wireframe\n";
    std::cout << "  Esc    Exit\n\n";

    std::cout << "Demo2D keys:\n";
    std::cout << "  W/A/S/D move, Q/E rotate, Z/X scale\n";
    std::cout << "  H/J, N/M shear, Y/U reflect, T order switch, R reset\n\n";

    std::cout << "Demo3D keys:\n";
    std::cout << "  W/A/S/D + Q/E camera move\n";
    std::cout << "  Arrows + Z/X rotate, I/J/K/L/O/U translate model\n";
    std::cout << "  +/- scale, P projection switch, T order switch, R reset\n";
}
}

int main(int argc, char** argv) {
    bool validateMode = false;
    bool helpMode = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--validate") {
            validateMode = true;
        } else if (arg == "--help" || arg == "-h") {
            helpMode = true;
        }
    }

    if (helpMode) {
        printHelp();
        return 0;
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(960, 720, "CG_ex1", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    try {
        App app(window);
        if (validateMode) {
            const std::string screenshotDir = std::string(PROJECT_SOURCE_DIR_PATH) + "/docs/screenshots";
            if (!app.runChecklistValidation(screenshotDir)) {
                std::cerr << "Validation run failed." << std::endl;
                glfwDestroyWindow(window);
                glfwTerminate();
                return -1;
            }
        } else {
            app.run();
        }
    } catch (const std::exception& ex) {
        std::cerr << "Runtime error: " << ex.what() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
