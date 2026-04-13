#include "scene/Demo2D.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <imgui.h>

namespace {
float clampScale(float v) {
    if (v < 0.1f) {
        return 0.1f;
    }
    if (v > 5.0f) {
        return 5.0f;
    }
    return v;
}

float clampValue(float v, float minV, float maxV) {
    if (v < minV) {
        return minV;
    }
    if (v > maxV) {
        return maxV;
    }
    return v;
}
}

Demo2D::Demo2D(const std::string& projectRoot)
    : shader_(projectRoot + "/shaders/vertex2d.glsl", projectRoot + "/shaders/fragment2d.glsl"),
      vao_(0),
      vbo_(0),
      tx_(0.0f),
      ty_(0.0f),
      rotRad_(0.0f),
      sx_(1.0f),
      sy_(1.0f),
      shx_(0.0f),
      shy_(0.0f),
      reflectX_(false),
      reflectY_(false),
      altOrder_(false) {
    const float vertices[] = {
        -0.35f, -0.35f, 0.0f, 1.0f, 0.2f, 0.2f,
         0.35f, -0.35f, 0.0f, 0.2f, 1.0f, 0.2f,
         0.35f,  0.35f, 0.0f, 0.2f, 0.2f, 1.0f,

        -0.35f, -0.35f, 0.0f, 1.0f, 0.2f, 0.2f,
         0.35f,  0.35f, 0.0f, 0.2f, 0.2f, 1.0f,
        -0.35f,  0.35f, 0.0f, 1.0f, 1.0f, 0.2f
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Demo2D::~Demo2D() {
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
}

void Demo2D::onEnter() {
    glDisable(GL_DEPTH_TEST);
}

void Demo2D::render(int width, int height) {
    const float aspect = (height == 0) ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
    const Mat4 model = buildModel();
    const Mat4 view = Mat4::identity();
    const Mat4 proj = Mat4::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    shader_.use();
    shader_.setFloat("u_Time", static_cast<float>(glfwGetTime()));
    shader_.setMat4("u_Model", model);
    shader_.setMat4("u_View", view);
    shader_.setMat4("u_Proj", proj);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Demo2D::onKey(int key, int action) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    const float moveStep = 0.015f;
    const float rotStep = degToRad(1.2f);
    const float scaleFactorUp = 1.015f;
    const float scaleFactorDown = 1.0f / scaleFactorUp;
    const float shearStep = 0.01f;

    if (key == GLFW_KEY_W) {
        ty_ += moveStep;
    } else if (key == GLFW_KEY_S) {
        ty_ -= moveStep;
    } else if (key == GLFW_KEY_A) {
        tx_ -= moveStep;
    } else if (key == GLFW_KEY_D) {
        tx_ += moveStep;
    } else if (key == GLFW_KEY_Q) {
        rotRad_ += rotStep;
    } else if (key == GLFW_KEY_E) {
        rotRad_ -= rotStep;
    } else if (key == GLFW_KEY_Z) {
        sx_ = clampScale(sx_ * scaleFactorDown);
        sy_ = clampScale(sy_ * scaleFactorDown);
    } else if (key == GLFW_KEY_X) {
        sx_ = clampScale(sx_ * scaleFactorUp);
        sy_ = clampScale(sy_ * scaleFactorUp);
    } else if (key == GLFW_KEY_H) {
        shx_ += shearStep;
    } else if (key == GLFW_KEY_J) {
        shx_ -= shearStep;
    } else if (key == GLFW_KEY_N) {
        shy_ += shearStep;
    } else if (key == GLFW_KEY_M) {
        shy_ -= shearStep;
    } else if (key == GLFW_KEY_Y && action == GLFW_PRESS) {
        reflectX_ = !reflectX_;
    } else if (key == GLFW_KEY_U && action == GLFW_PRESS) {
        reflectY_ = !reflectY_;
    } else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        altOrder_ = !altOrder_;
        std::cout << "[Demo2D] 组合顺序: " << (altOrder_ ? "R*T*..." : "T*R*...") << std::endl;
    } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        resetTransform();
    }

    // Keep object in a comfortable visible region for interaction demonstrations.
    tx_ = clampValue(tx_, -0.65f, 0.65f);
    ty_ = clampValue(ty_, -0.65f, 0.65f);
    shx_ = clampValue(shx_, -0.8f, 0.8f);
    shy_ = clampValue(shy_, -0.8f, 0.8f);
}

void Demo2D::resetTransform() {
    tx_ = 0.0f;
    ty_ = 0.0f;
    rotRad_ = 0.0f;
    sx_ = 1.0f;
    sy_ = 1.0f;
    shx_ = 0.0f;
    shy_ = 0.0f;
    reflectX_ = false;
    reflectY_ = false;
    altOrder_ = false;
}

Mat4 Demo2D::buildModel() const {
    const Mat4 t = Mat4::translate(tx_, ty_, 0.0f);
    const Mat4 r = Mat4::rotateZ(rotRad_);
    const Mat4 s = Mat4::scale(sx_, sy_, 1.0f);
    const Mat4 sh = Mat4::shearXY(shx_, shy_);

    Mat4 reflect = Mat4::identity();
    if (reflectX_) {
        reflect = Mat4::reflectX() * reflect;
    }
    if (reflectY_) {
        reflect = Mat4::reflectY() * reflect;
    }

    if (!altOrder_) {
        return t * r * sh * reflect * s;
    }
    return r * t * sh * reflect * s;
}

void Demo2D::resetParameters() {
    resetTransform();
}
void Demo2D::drawUi() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 360.0f, 20.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.92f);
    ImGui::Begin(
            "Coordinates",
            nullptr,
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::TextUnformatted("2D scene: no 3D camera");
    ImGui::BulletText("View matrix: identity");
    ImGui::BulletText("Projection: ortho (NDC plane)");
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.92f, 0.75f, 1.0f));
    ImGui::TextUnformatted("Object");
    ImGui::PopStyleColor();
    ImGui::BulletText("Translation (tx, ty): %.4f , %.4f", tx_, ty_);
    ImGui::BulletText("Rotation Z (deg):     %.2f", rotRad_ * 180.0f / 3.14159265f);
    ImGui::BulletText("Scale (sx, sy):       %.4f , %.4f", sx_, sy_);
    ImGui::BulletText("Shear (shx, shy):      %.4f , %.4f", shx_, shy_);
    ImGui::Separator();
    ImGui::TextUnformatted("Flags");
    ImGui::BulletText("Reflect X: %s  Reflect Y: %s", reflectX_ ? "on" : "off", reflectY_ ? "on" : "off");
    ImGui::BulletText("Combine alt order: %s", altOrder_ ? "R*T" : "T*R");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(360.0f, 620.0f), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Demo2D / Properties", nullptr)) {
        ImGui::End();
        return;
    }

    auto DrawAESlider = [](const char* label, const char* shortcut, float* v, float v_min, float v_max, const char* format = "%.3f") {
        ImGui::Text("%s %s", label, shortcut ? shortcut : "");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60.0f);
        ImGui::TextDisabled(format, *v);

        ImGui::PushID(label);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderFloat("##slider", v, v_min, v_max, format);
        ImGui::PopID();
        ImGui::Spacing();
    };

    if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        DrawAESlider("Position X", "(A/D)", &tx_, -0.65f, 0.65f);
        DrawAESlider("Position Y", "(W/S)", &ty_, -0.65f, 0.65f);

        float rotDeg = rotRad_ * 180.0f / 3.14159265f;
        ImGui::Text("Rotation Z %s", "(Q/E)");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70.0f);
        ImGui::TextDisabled("%.1f deg", rotDeg);
        ImGui::PushID("Rotation Z");
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##slider", &rotDeg, -180.0f, 180.0f, "%.1f deg")) {
            rotRad_ = degToRad(rotDeg);
        }
        ImGui::PopID();
        ImGui::Spacing();

        // 源码逻辑中 Z/X 是同时缩放 X 和 Y，所以将快捷键标记在它们旁边
        DrawAESlider("Scale X", "(Z/X)", &sx_, 0.1f, 5.0f);
        DrawAESlider("Scale Y", "(Z/X)", &sy_, 0.1f, 5.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Shear", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        DrawAESlider("Shear shx", "(H/J)", &shx_, -0.8f, 0.8f);
        DrawAESlider("Shear shy", "(N/M)", &shy_, -0.8f, 0.8f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Reflect & combine", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Reflect across X axis (Y)", &reflectX_);
        ImGui::Checkbox("Reflect across Y axis (U)", &reflectY_);
        ImGui::Checkbox("Alt order (R*T vs T*R) (T)", &altOrder_);
        ImGui::TreePop();
    }

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Reset All Parameters (R)", ImVec2(-1.0f, 36.0f))) {
        resetParameters();
    }

    ImGui::End();
}