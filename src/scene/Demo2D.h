#pragma once

#include <string>

#include "core/Shader.h"
#include "scene/Scene.h"

class Demo2D : public Scene {
public:
    explicit Demo2D(const std::string& projectRoot);
    ~Demo2D() override;

    void onEnter() override;
    void render(int width, int height) override;
    void onKey(int key, int action) override;

    bool isAltOrder() const;

private:
    Shader shader_;
    unsigned int vao_;
    unsigned int vbo_;

    float tx_;
    float ty_;
    float rotRad_;
    float sx_;
    float sy_;
    float shx_;
    float shy_;
    bool reflectX_;
    bool reflectY_;
    bool altOrder_;

    void resetTransform();
    Mat4 buildModel() const;
};

inline bool Demo2D::isAltOrder() const {
    return altOrder_;
}
