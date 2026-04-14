#include "scene/Demo3D.h"

#include "mesh/SimpleObj.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include <imgui.h>

namespace {
float clampScale(float v) {
    if (v < 0.2f) {
        return 0.2f;
    }
    if (v > 4.0f) {
        return 4.0f;
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

Demo3D::Demo3D(const std::string& projectRoot)
    : shader_(projectRoot + "/shaders/vertex3d.glsl", projectRoot + "/shaders/fragment3d.glsl"),
      projectRoot_(projectRoot),
      vao_(0),
      vbo_(0),
      ebo_(0),
      cameraEye_(0.0f, 0.0f, 3.0f),
      cameraCenter_(0.0f, 0.0f, 0.0f),
      cameraUp_(0.0f, 1.0f, 0.0f),
      posX_(0.0f),
      posY_(0.0f),
      posZ_(0.0f),
      // 初始略转一点，否则相机在 +Z 轴正视时只能看到朝前的一个面，会像 2D 方块
      rotX_(degToRad(22.0f)),
      rotY_(degToRad(-32.0f)),
      rotZ_(0.0f),
      scale_(1.0f),
      perspective_(true),
      altOrder_(false) {
    strncpy(objPathBuf_, "models/cube.obj", sizeof(objPathBuf_) - 1);
}

Demo3D::~Demo3D() {
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
    }
}

void Demo3D::onEnter() {
    glEnable(GL_DEPTH_TEST);
}

void Demo3D::render(int width, int height) {
    if (indexCount_ == 0) {
        return;
    }

    const float aspect = (height == 0) ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
    const Mat4 model = buildModel();
    const Mat4 view = Mat4::lookAt(cameraEye_, cameraCenter_, cameraUp_);

    Mat4 proj;
    if (perspective_) {
        proj = Mat4::perspective(degToRad(60.0f), aspect, 0.1f, 100.0f);
    } else {
        proj = Mat4::ortho(-2.0f * aspect, 2.0f * aspect, -2.0f, 2.0f, 0.1f, 100.0f);
    }

    shader_.use();
    shader_.setFloat("u_Time", static_cast<float>(glfwGetTime()));
    shader_.setMat4("u_Model", model);
    shader_.setMat4("u_View", view);
    shader_.setMat4("u_Proj", proj);

    glBindVertexArray(vao_);
    shader_.setFloat("u_IsWireframe", 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount_), GL_UNSIGNED_INT, nullptr);

    glDisable(GL_POLYGON_OFFSET_FILL);

    if (drawEdge_) {
        shader_.setFloat("u_IsWireframe", 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount_), GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Demo3D::onKey(int key, int action) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    const float cameraStep = 0.03f;
    const float modelStep = 0.03f;
    const float rotStep = degToRad(1.2f);
    const float scaleFactorUp = 1.012f;
    const float scaleFactorDown = 1.0f / scaleFactorUp;

    if (key == GLFW_KEY_W) {
        cameraEye_.z -= cameraStep;
    } else if (key == GLFW_KEY_S) {
        cameraEye_.z += cameraStep;
    } else if (key == GLFW_KEY_A) {
        cameraEye_.x -= cameraStep;
    } else if (key == GLFW_KEY_D) {
        cameraEye_.x += cameraStep;
    } else if (key == GLFW_KEY_Q) {
        cameraEye_.y += cameraStep;
    } else if (key == GLFW_KEY_E) {
        cameraEye_.y -= cameraStep;
    } else if (key == GLFW_KEY_UP) {
        rotX_ += rotStep;
    } else if (key == GLFW_KEY_DOWN) {
        rotX_ -= rotStep;
    } else if (key == GLFW_KEY_LEFT) {
        rotY_ += rotStep;
    } else if (key == GLFW_KEY_RIGHT) {
        rotY_ -= rotStep;
    } else if (key == GLFW_KEY_Z) {
        rotZ_ += rotStep;
    } else if (key == GLFW_KEY_X) {
        rotZ_ -= rotStep;
    } else if (key == GLFW_KEY_KP_ADD || key == GLFW_KEY_EQUAL) {
        scale_ = clampScale(scale_ * scaleFactorUp);
    } else if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) {
        scale_ = clampScale(scale_ * scaleFactorDown);
    } else if (key == GLFW_KEY_I) {
        posY_ += modelStep;
    } else if (key == GLFW_KEY_K) {
        posY_ -= modelStep;
    } else if (key == GLFW_KEY_J) {
        posX_ -= modelStep;
    } else if (key == GLFW_KEY_L) {
        posX_ += modelStep;
    } else if (key == GLFW_KEY_O) {
        posZ_ += modelStep;
    } else if (key == GLFW_KEY_U) {
        posZ_ -= modelStep;
    } else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        perspective_ = !perspective_;
        std::cout << "[Demo3D] 投影模式: " << (perspective_ ? "Perspective" : "Ortho") << std::endl;
    } else if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        altOrder_ = !altOrder_;
        std::cout << "[Demo3D] 组合顺序: " << (altOrder_ ? "R*T*S" : "T*R*S") << std::endl;
    } else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        resetTransform();
    }

    // Keep camera and model within a comfortable demonstration range.
    cameraEye_.x = clampValue(cameraEye_.x, -3.5f, 3.5f);
    cameraEye_.y = clampValue(cameraEye_.y, -2.5f, 2.5f);
    cameraEye_.z = clampValue(cameraEye_.z, 1.2f, 8.0f);

    posX_ = clampValue(posX_, -1.2f, 1.2f);
    posY_ = clampValue(posY_, -1.2f, 1.2f);
    posZ_ = clampValue(posZ_, -1.2f, 1.2f);

    // Keep view centered on the model for easier observation while moving camera.
    cameraCenter_ = Vec3(0.0f, 0.0f, 0.0f);
}

void Demo3D::resetTransform() {
    cameraEye_ = Vec3(0.0f, 0.0f, 3.0f);
    cameraCenter_ = Vec3(0.0f, 0.0f, 0.0f);
    cameraUp_ = Vec3(0.0f, 1.0f, 0.0f);

    posX_ = 0.0f;
    posY_ = 0.0f;
    posZ_ = 0.0f;
    rotX_ = 0.0f;
    rotY_ = 0.0f;
    rotZ_ = 0.0f;
    scale_ = 1.0f;
    perspective_ = true;
    altOrder_ = false;
}

Mat4 Demo3D::buildModel() const {
    const Mat4 t = Mat4::translate(posX_, posY_, posZ_);
    const Mat4 s = Mat4::scale(scale_, scale_, scale_);
    const Mat4 r = Mat4::rotateZ(rotZ_) * Mat4::rotateY(rotY_) * Mat4::rotateX(rotX_);

    if (!altOrder_) {
        return t * r * s;
    }
    return r * t * s;
}

void Demo3D::resetParameters() {
    resetTransform();
}

void Demo3D::loadModel(const std::string& path) {
    SimpleObjMesh objMesh;
    std::string finalPath = path;

    if (!loadSimpleObjFile(finalPath, objMesh)) {
        finalPath = projectRoot_ + "/" + path;
        loadSimpleObjFile(finalPath, objMesh);
    }

    if (objMesh.valid) {
        uploadMeshToGpu(objMesh.vertices, objMesh.indices);
        std::cout << "[Demo3D] 成功导入: " << finalPath << std::endl;
        strncpy(objPathBuf_, path.c_str(), sizeof(objPathBuf_) - 1);
    } else {
        std::cerr << "[Demo3D] 导入失败，请检查文件: " << path << std::endl;
    }
}

void Demo3D::drawUi() {
    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 360.0f, 20.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.92f);
    ImGui::Begin(
            "Coordinates",
            nullptr,
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

    if (ImGui::TreeNodeEx(">> MODEL IMPORTER", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();

        ImGui::TextDisabled("PATH (Drag & Drop .obj here!)");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputText("##objPath", objPathBuf_, sizeof(objPathBuf_));

        if (ImGui::Button("[ LOAD OBJ FILE ]", ImVec2(-1.0f, 36.0f))) {
            loadModel(std::string(objPathBuf_));
        }

        ImGui::Spacing();
        ImGui::Checkbox("Draw Black Edge Overlay", &drawEdge_);
        ImGui::TreePop();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.92f, 0.95f, 1.0f, 1.0f));
    ImGui::TextUnformatted("Camera");
    ImGui::PopStyleColor();
    ImGui::BulletText("Eye (x, y, z):  %.4f   %.4f   %.4f", cameraEye_.x, cameraEye_.y, cameraEye_.z);
    ImGui::BulletText("Look-at center: %.4f   %.4f   %.4f", cameraCenter_.x, cameraCenter_.y, cameraCenter_.z);
    ImGui::BulletText("Up:             %.4f   %.4f   %.4f", cameraUp_.x, cameraUp_.y, cameraUp_.z);
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.92f, 0.75f, 1.0f));
    ImGui::TextUnformatted("Object");
    ImGui::PopStyleColor();
    ImGui::BulletText("Position (world): %.4f   %.4f   %.4f", posX_, posY_, posZ_);
    ImGui::BulletText("Rotation (deg):   %.2f   %.2f   %.2f",
                      rotX_ * 180.0f / 3.14159265f,
                      rotY_ * 180.0f / 3.14159265f,
                      rotZ_ * 180.0f / 3.14159265f);
    ImGui::BulletText("Uniform scale:      %.4f", scale_);
    ImGui::Separator();
    ImGui::TextUnformatted("Projection");
    ImGui::BulletText("%s", perspective_ ? "Perspective" : "Orthographic");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(380.0f, 720.0f), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("SYSTEM CONTROL // Demo3D", nullptr)) {
        ImGui::End();
        return;
    }

    constexpr ImGuiSliderFlags kSlide = ImGuiSliderFlags_AlwaysClamp;

    auto DrawAESlider = [](const char* label, const char* shortcut, float* v, float v_min, float v_max, const char* format = "%.2f", ImGuiSliderFlags flags = 0) {
        ImGui::Text("%s %s", label, shortcut ? shortcut : "");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 60.0f);
        ImGui::TextDisabled(format, *v);

        ImGui::PushID(label);
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderFloat("##slider", v, v_min, v_max, format, flags);
        ImGui::PopID();
        ImGui::Spacing();
    };

    auto DrawAEAngleSlider = [&](const char* label, const char* shortcut, float* rad_v, float deg_min, float deg_max) {
        float deg = *rad_v * 180.0f / 3.14159265f;
        ImGui::Text("%s %s", label, shortcut ? shortcut : "");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70.0f);
        ImGui::TextDisabled("%.1f deg", deg);
        ImGui::PushID(label);
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderFloat("##slider", &deg, deg_min, deg_max, "%.1f deg", kSlide)) {
            *rad_v = deg * 3.14159265f / 180.0f;
        }
        ImGui::PopID();
        ImGui::Spacing();
    };

    if (ImGui::TreeNodeEx(">> CAMERA OPTICS", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        DrawAESlider("Eye X", "(A/D)", &cameraEye_.x, -7.5f, 7.5f, "%.2f", kSlide);
        DrawAESlider("Eye Y", "(Q/E)", &cameraEye_.y, -5.5f, 5.5f, "%.2f", kSlide);
        DrawAESlider("Eye Z", "(W/S)", &cameraEye_.z, 1.2f, 8.0f, "%.2f", kSlide);
        ImGui::Spacing();
        ImGui::TextDisabled("TARGET: (%.2f, %.2f, %.2f)", cameraCenter_.x, cameraCenter_.y, cameraCenter_.z);
        ImGui::TextDisabled("UP VEC: (%.2f, %.2f, %.2f)", cameraUp_.x, cameraUp_.y, cameraUp_.z);
        ImGui::TreePop();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::TreeNodeEx(">> MODEL KINEMATICS", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        DrawAESlider("Pos X", "(J/L)", &posX_, -1.2f, 1.2f, "%.3f", kSlide);
        DrawAESlider("Pos Y", "(I/K)", &posY_, -1.2f, 1.2f, "%.3f", kSlide);
        DrawAESlider("Pos Z", "(U/O)", &posZ_, -1.2f, 1.2f, "%.3f", kSlide);

        ImGui::Spacing();
        DrawAEAngleSlider("Rot X", "(UP/DOWN)", &rotX_, -180.0f, 180.0f);
        DrawAEAngleSlider("Rot Y", "(LEFT/RIGHT)", &rotY_, -180.0f, 180.0f);
        DrawAEAngleSlider("Rot Z", "(Z/X)", &rotZ_, -180.0f, 180.0f);

        ImGui::Spacing();
        DrawAESlider("Uniform Scale", "(-/+)", &scale_, 0.2f, 4.0f, "%.3f", kSlide);
        ImGui::TreePop();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::TreeNodeEx(">> RENDER PIPELINE", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextUnformatted("PROJECTION MATRIX (P)");
        int projMode = perspective_ ? 0 : 1;
        if (ImGui::RadioButton("Perspective", &projMode, 0)) perspective_ = true;
        ImGui::SameLine();
        if (ImGui::RadioButton("Orthographic", &projMode, 1)) perspective_ = false;

        ImGui::Spacing();

        ImGui::TextUnformatted("TRANSFORM ORDER (T)");
        int orderMode = altOrder_ ? 1 : 0;
        if (ImGui::RadioButton("T * R * S (Standard)", &orderMode, 0)) altOrder_ = false;
        if (ImGui::RadioButton("R * T * S (Alt)", &orderMode, 1)) altOrder_ = true;

        ImGui::TreePop();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    if (ImGui::Button("Reset All Parameters (R)", ImVec2(-1.0f, 36.0f))) {
        resetParameters();
    }

    ImGui::End();
}

void Demo3D::uploadMeshToGpu(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    if (vao_ == 0) {
        glGenVertexArrays(1, &vao_);
    }
    if (vbo_ == 0) {
        glGenBuffers(1, &vbo_);
    }
    if (ebo_ == 0) {
        glGenBuffers(1, &ebo_);
    }

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    indexCount_ = indices.size();
}
