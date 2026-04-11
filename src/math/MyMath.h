#pragma once

#include <cmath>
#include <cstring>

// Column-major matrix math for OpenGL.
// Storage uses m[col * 4 + row], compatible with glUniformMatrix4fv(..., GL_FALSE, m).
struct Vec3 {
    float x;
    float y;
    float z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float xIn, float yIn, float zIn) : x(xIn), y(yIn), z(zIn) {}

    Vec3 operator+(const Vec3& rhs) const {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vec3 operator-(const Vec3& rhs) const {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vec3 operator*(float s) const {
        return Vec3(x * s, y * s, z * s);
    }

    Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
};

inline float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

inline Vec3 normalize(const Vec3& v) {
    const float len = std::sqrt(dot(v, v));
    if (len <= 1e-6f) {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    const float inv = 1.0f / len;
    return Vec3(v.x * inv, v.y * inv, v.z * inv);
}

struct Mat4 {
    float m[16];

    Mat4() {
        std::memset(m, 0, sizeof(m));
        m[0] = 1.0f;
        m[5] = 1.0f;
        m[10] = 1.0f;
        m[15] = 1.0f;
    }

    static Mat4 zero() {
        Mat4 mat;
        std::memset(mat.m, 0, sizeof(mat.m));
        return mat;
    }

    static Mat4 identity() {
        return Mat4();
    }

    float& at(int row, int col) {
        return m[col * 4 + row];
    }

    const float& at(int row, int col) const {
        return m[col * 4 + row];
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 result = Mat4::zero();
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += at(i, k) * other.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }

    static Mat4 translate(float tx, float ty, float tz) {
        Mat4 mat = Mat4::identity();
        mat.at(0, 3) = tx;
        mat.at(1, 3) = ty;
        mat.at(2, 3) = tz;
        return mat;
    }

    static Mat4 scale(float sx, float sy, float sz) {
        Mat4 mat = Mat4::identity();
        mat.at(0, 0) = sx;
        mat.at(1, 1) = sy;
        mat.at(2, 2) = sz;
        return mat;
    }

    // Angle unit: radians.
    static Mat4 rotateX(float angleRadians) {
        Mat4 mat = Mat4::identity();
        const float c = std::cos(angleRadians);
        const float s = std::sin(angleRadians);
        mat.at(1, 1) = c;
        mat.at(1, 2) = -s;
        mat.at(2, 1) = s;
        mat.at(2, 2) = c;
        return mat;
    }

    // Angle unit: radians.
    static Mat4 rotateY(float angleRadians) {
        Mat4 mat = Mat4::identity();
        const float c = std::cos(angleRadians);
        const float s = std::sin(angleRadians);
        mat.at(0, 0) = c;
        mat.at(0, 2) = s;
        mat.at(2, 0) = -s;
        mat.at(2, 2) = c;
        return mat;
    }

    // Angle unit: radians.
    static Mat4 rotateZ(float angleRadians) {
        Mat4 mat = Mat4::identity();
        const float c = std::cos(angleRadians);
        const float s = std::sin(angleRadians);
        mat.at(0, 0) = c;
        mat.at(0, 1) = -s;
        mat.at(1, 0) = s;
        mat.at(1, 1) = c;
        return mat;
    }

    // 2D shear on XY plane:
    // x' = x + shx * y
    // y' = y + shy * x
    static Mat4 shearXY(float shx, float shy) {
        Mat4 mat = Mat4::identity();
        mat.at(0, 1) = shx;
        mat.at(1, 0) = shy;
        return mat;
    }

    // Reflection across X axis in XY plane: (x, y) -> (x, -y).
    static Mat4 reflectX() {
        Mat4 mat = Mat4::identity();
        mat.at(1, 1) = -1.0f;
        return mat;
    }

    // Reflection across Y axis in XY plane: (x, y) -> (-x, y).
    static Mat4 reflectY() {
        Mat4 mat = Mat4::identity();
        mat.at(0, 0) = -1.0f;
        return mat;
    }

    // Perspective projection matrix (OpenGL NDC z in [-1, 1]).
    // fovyRad unit: radians.
    static Mat4 perspective(float fovyRad, float aspect, float znear, float zfar) {
        Mat4 mat = Mat4::zero();
        const float f = 1.0f / std::tan(fovyRad * 0.5f);

        mat.at(0, 0) = f / aspect;
        mat.at(1, 1) = f;
        mat.at(2, 2) = (zfar + znear) / (znear - zfar);
        mat.at(2, 3) = (2.0f * zfar * znear) / (znear - zfar);
        mat.at(3, 2) = -1.0f;
        return mat;
    }

    // Orthographic projection matrix (OpenGL NDC z in [-1, 1]).
    static Mat4 ortho(float left, float right, float bottom, float top, float znear, float zfar) {
        Mat4 mat = Mat4::identity();
        mat.at(0, 0) = 2.0f / (right - left);
        mat.at(1, 1) = 2.0f / (top - bottom);
        mat.at(2, 2) = -2.0f / (zfar - znear);

        mat.at(0, 3) = -(right + left) / (right - left);
        mat.at(1, 3) = -(top + bottom) / (top - bottom);
        mat.at(2, 3) = -(zfar + znear) / (zfar - znear);
        return mat;
    }

    // Right-handed LookAt view matrix for column-major storage.
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        const Vec3 f = normalize(center - eye);
        const Vec3 s = normalize(cross(f, up));
        const Vec3 u = cross(s, f);

        Mat4 mat = Mat4::identity();
        mat.at(0, 0) = s.x;
        mat.at(1, 0) = s.y;
        mat.at(2, 0) = s.z;

        mat.at(0, 1) = u.x;
        mat.at(1, 1) = u.y;
        mat.at(2, 1) = u.z;

        mat.at(0, 2) = -f.x;
        mat.at(1, 2) = -f.y;
        mat.at(2, 2) = -f.z;

        mat.at(0, 3) = -dot(s, eye);
        mat.at(1, 3) = -dot(u, eye);
        mat.at(2, 3) = dot(f, eye);
        return mat;
    }
};

inline float degToRad(float degree) {
    return degree * 3.14159265358979323846f / 180.0f;
}
