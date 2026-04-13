#include <cstdlib>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "BuildConfig.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "app/App.h"

namespace {
/// ImGui's built-in font has no CJK glyphs; load a system font on Windows or fall back to default.
void setupImGuiFonts(ImGuiIO& io) {
#ifdef _WIN32
    const char* windir = std::getenv("WINDIR");
    const std::string fontDir = windir ? (std::string(windir) + "\\Fonts\\") : std::string("C:\\Windows\\Fonts\\");
    const char* candidates[] = {
        "msyh.ttc",   // Microsoft YaHei
        "simhei.ttf", // Hei
        "simsun.ttc", // Song
    };
    for (const char* name : candidates) {
        const std::string path = fontDir + name;
        ImFont* loaded = io.Fonts->AddFontFromFileTTF(
            path.c_str(),
            18.0f,
            nullptr,
            io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        if (loaded != nullptr) {
            return;
        }
    }
    std::cerr << "[ImGui] No Chinese-capable system font found under WINDIR\\Fonts; CJK labels may show as '?'\n";
#endif
    io.Fonts->AddFontDefault();
}

/// Cyberpunk / HUD: sharp corners, framed controls, neon cyan accents.
void applyImGuiSciFiStyle() {
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 0.0f;
    s.ChildRounding = 0.0f;
    s.FrameRounding = 0.0f;
    s.GrabRounding = 0.0f;
    s.TabRounding = 0.0f;

    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize = 1.0f;
    s.GrabMinSize = 20.0f;

    ImVec4* c = s.Colors;
    const ImVec4 bgDeep(0.04f, 0.05f, 0.08f, 0.95f);
    const ImVec4 frameBg(0.08f, 0.12f, 0.18f, 1.0f);
    const ImVec4 neonCyan(0.0f, 0.85f, 1.0f, 1.0f);
    const ImVec4 cyanDim(0.0f, 0.45f, 0.60f, 1.0f);

    c[ImGuiCol_Text] = ImVec4(0.85f, 0.95f, 1.0f, 1.0f);
    c[ImGuiCol_WindowBg] = bgDeep;

    c[ImGuiCol_Border] = cyanDim;
    c[ImGuiCol_FrameBg] = frameBg;
    c[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.18f, 0.25f, 1.0f);
    c[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.22f, 0.30f, 1.0f);

    c[ImGuiCol_SliderGrab] = cyanDim;
    c[ImGuiCol_SliderGrabActive] = neonCyan;

    c[ImGuiCol_Button] = cyanDim;
    c[ImGuiCol_ButtonHovered] = neonCyan;
    c[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.95f, 1.0f, 1.0f);
    c[ImGuiCol_Header] = cyanDim;
    c[ImGuiCol_HeaderHovered] = neonCyan;
    c[ImGuiCol_CheckMark] = neonCyan;
}

void printHelp() {
    std::cout << "CG_ex1 usage:\n";
    std::cout << "  CG_ex1.exe              Run interactive demo (starts in 2D)\n";
    std::cout << "  CG_ex1.exe --2d         Same as default: open in Demo2D\n";
    std::cout << "  CG_ex1.exe --3d         Open in Demo3D (3D cube)\n";
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
    std::cout << "\nImGui property panel: sliders for transforms / camera (focus off UI to use game keys).\n";
}
}

int main(int argc, char** argv) {
    bool validateMode = false;
    bool helpMode = false;
    bool startWithDemo3D = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--validate") {
            validateMode = true;
        } else if (arg == "--help" || arg == "-h") {
            helpMode = true;
        } else if (arg == "--3d") {
            startWithDemo3D = true;
        } else if (arg == "--2d") {
            startWithDemo3D = false;
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

    GLFWwindow* window = glfwCreateWindow(1600, 900, "CG_ex1", nullptr, nullptr);
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imguiIo = ImGui::GetIO();
    imguiIo.IniFilename = nullptr;
    setupImGuiFonts(imguiIo);
    applyImGuiSciFiStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 330");

    int exitCode = 0;
    try {
        App app(window, startWithDemo3D);
        if (validateMode) {
            const std::string screenshotDir = std::string(PROJECT_SOURCE_DIR_PATH) + "/docs/screenshots";
            if (!app.runChecklistValidation(screenshotDir)) {
                std::cerr << "Validation run failed." << std::endl;
                exitCode = -1;
            }
        } else {
            app.run();
        }
    } catch (const std::exception& ex) {
        std::cerr << "Runtime error: " << ex.what() << std::endl;
        exitCode = -1;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return exitCode;
}
