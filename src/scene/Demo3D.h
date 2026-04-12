#pragma once

#include <string>

#include "core/Shader.h"
#include "scene/Scene.h"

class Demo3D : public Scene {
public:
    explicit Demo3D(const std::string& projectRoot);
    ~Demo3D() override;

    void onEnter() override;
    void render(int width, int height) override;
    void onKey(int key, int action) override;
    void drawUi() override;

    bool isPerspective() const;
    bool isAltOrder() const;
    Vec3 cameraEye() const;
    void resetParameters();

private:
    Shader shader_;
    unsigned int vao_;
    unsigned int vbo_;

    Vec3 cameraEye_;
    Vec3 cameraCenter_;
    Vec3 cameraUp_;

    float posX_;
    float posY_;
    float posZ_;
    float rotX_;
    float rotY_;
    float rotZ_;
    float scale_;
    bool perspective_;
    bool altOrder_;

    void resetTransform();
    Mat4 buildModel() const;
};

inline bool Demo3D::isPerspective() const {
    return perspective_;
}

inline bool Demo3D::isAltOrder() const {
    return altOrder_;
}

inline Vec3 Demo3D::cameraEye() const {
    return cameraEye_;
}
