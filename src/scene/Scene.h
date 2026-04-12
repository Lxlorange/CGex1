#pragma once

class Scene {
public:
    virtual ~Scene() = default;

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void update(float deltaTime) {}
    virtual void render(int width, int height) = 0;
    virtual void onKey(int key, int action) {}
    /// ImGui property panel (optional).
    virtual void drawUi() {}
};
