#include "scene/Demo3D.h"

#include "mesh/SimpleObj.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

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
      vao_(0),
      vbo_(0),
      ebo_(0),
      cameraEye_(0.0f, 0.0f, 3.0f),
      cameraCenter_(0.0f, 0.0f, 0.0f),
      cameraUp_(0.0f, 1.0f, 0.0f),
      posX_(0.0f),
      posY_(0.0f),
      posZ_(0.0f),
      rotX_(0.0f),
      rotY_(0.0f),
      rotZ_(0.0f),
      scale_(1.0f),
      perspective_(true),
      altOrder_(false) {
    const std::string objPath = projectRoot + "/assets/models/cube.obj";
    SimpleObjMesh objMesh;
    if (loadSimpleObjFile(objPath, objMesh) && objMesh.valid) {
        uploadMeshToGpu(objMesh.vertices, objMesh.indices);
    } else {
        std::cerr << "[Demo3D] OBJ 加载失败，使用内置立方体: " << objPath << std::endl;
        // 与原先一致：24 顶点 + 索引（每面独立颜色）
        static const float vertices[] = {
            -0.5f, -0.5f, -0.5f, 1.0f, 0.2f, 0.2f,
            0.5f, -0.5f, -0.5f, 0.2f, 1.0f, 0.2f,
            0.5f, 0.5f, -0.5f, 0.2f, 0.2f, 1.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.2f,
            -0.5f, -0.5f, 0.5f, 1.0f, 0.2f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.2f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.8f, 0.8f, 0.3f,
            -0.5f, 0.5f, 0.5f, 0.9f, 0.3f, 0.4f,
            -0.5f, 0.5f, -0.5f, 0.2f, 0.9f, 0.4f,
            -0.5f, -0.5f, -0.5f, 0.2f, 0.4f, 0.9f,
            -0.5f, -0.5f, 0.5f, 0.9f, 0.8f, 0.2f,
            0.5f, 0.5f, 0.5f, 0.4f, 0.7f, 0.9f,
            0.5f, 0.5f, -0.5f, 0.9f, 0.4f, 0.7f,
            0.5f, -0.5f, -0.5f, 0.7f, 0.9f, 0.4f,
            0.5f, -0.5f, 0.5f, 0.4f, 0.9f, 0.7f,
            -0.5f, -0.5f, -0.5f, 0.6f, 0.2f, 0.7f,
            0.5f, -0.5f, -0.5f, 0.3f, 0.8f, 0.7f,
            0.5f, -0.5f, 0.5f, 0.8f, 0.3f, 0.7f,
            -0.5f, -0.5f, 0.5f, 0.7f, 0.7f, 0.2f,
            -0.5f, 0.5f, -0.5f, 0.1f, 0.9f, 0.9f,
            0.5f, 0.5f, -0.5f, 0.9f, 0.1f, 0.9f,
            0.5f, 0.5f, 0.5f, 0.9f, 0.9f, 0.1f,
            -0.5f, 0.5f, 0.5f, 0.1f, 0.9f, 0.1f,
        };
        static const unsigned int indices[] = {
            0,  1,  2,  2,  3,  0,   //
            4,  5,  6,  6,  7,  4,   //
            8,  9,  10, 10, 11, 8,   //
            12, 13, 14, 14, 15, 12,  //
            16, 17, 18, 18, 19, 16,  //
            20, 21, 22, 22, 23, 20,  //
        };
        uploadMeshToGpu(std::vector<float>(vertices, vertices + (sizeof(vertices) / sizeof(vertices[0]))),
                        std::vector<unsigned int>(indices, indices + (sizeof(indices) / sizeof(indices[0]))));
    }
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
    shader_.use();
    shader_.setFloat("u_Time", static_cast<float>(glfwGetTime()));

    const float aspect = (height == 0) ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
    const Mat4 model = buildModel();
    const Mat4 view = Mat4::lookAt(cameraEye_, cameraCenter_, cameraUp_);

    Mat4 proj;
    if (perspective_) {
        proj = Mat4::perspective(degToRad(60.0f), aspect, 0.1f, 100.0f);
    } else {
        proj = Mat4::ortho(-2.0f * aspect, 2.0f * aspect, -2.0f, 2.0f, 0.1f, 100.0f);
    }

    shader_.setMat4("u_Model", model);
    shader_.setMat4("u_View", view);
    shader_.setMat4("u_Proj", proj);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount_), GL_UNSIGNED_INT, nullptr);
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

void Demo3D::uploadMeshToGpu(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    indexCount_ = indices.size();
    if (indexCount_ == 0 || vertices.empty()) {
        return;
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
        indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
