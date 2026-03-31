#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "MyMath.h"

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(600, 600, "2D Transformation Test", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // 获取时间来做动态旋转的变量
        float timeValue = glfwGetTime();

        // 复合变换测试 M = T * R * S
        Mat4 S = Mat4::scale(0.5f, 0.5f, 1.0f);
        Mat4 R = Mat4::rotateZ(timeValue);
        Mat4 T = Mat4::translate(1.0f, 1.0f, 0.0f);

        Mat4 transform = T * R * S;

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(transform.m);

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0.0f, 0.5f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.5f, -0.5f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}