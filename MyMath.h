#pragma once
#include <cstring>
#include <cmath>

struct Mat4 {
    float m[16];

    Mat4() {
        memset(m, 0, sizeof(m));
        m[0]  = 1.0f;
        m[5]  = 1.0f;
        m[10] = 1.0f;
        m[15] = 1.0f;
    }

    float& at(int row, int col) {
        return m[col * 4 + row];
    }

    const float& at(int row, int col) const {
        return m[col * 4 + row];
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += this->at(i, k) * other.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }

    static Mat4 translate(float tx, float ty, float tz) {
        Mat4 mat;
        mat.at(0, 3) = tx;
        mat.at(1, 3) = ty;
        mat.at(2, 3) = tz;
        return mat;
    }

    static Mat4 scale(float sx, float sy, float sz) {
        Mat4 mat;
        mat.at(0, 0) = sx;
        mat.at(1, 1) = sy;
        mat.at(2, 2) = sz;
        return mat;
    }

    static Mat4 rotateX(float angleRadians) {
        Mat4 mat;
        float c = cos(angleRadians);
        float s = sin(angleRadians);
        mat.at(1, 1) = c;
        mat.at(1, 2) = -s;
        mat.at(2, 1) = s;
        mat.at(2, 2) = c;
        return mat;
    }

    static Mat4 rotateY(float angleRadians) {
        Mat4 mat;
        float c = cos(angleRadians);
        float s = sin(angleRadians);
        mat.at(0, 0) = c;
        mat.at(0, 2) = s;
        mat.at(2, 0) = -s;
        mat.at(2, 2) = c;
        return mat;
    }

    static Mat4 rotateZ(float angleRadians) {
        Mat4 mat;
        float c = cos(angleRadians);
        float s = sin(angleRadians);
        mat.at(0, 0) = c;
        mat.at(0, 1) = -s;
        mat.at(1, 0) = s;
        mat.at(1, 1) = c;
        return mat;
    }
};