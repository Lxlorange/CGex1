#pragma once

#include <string>

#include "math/MyMath.h"

class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() const;

    void setInt(const std::string& name, int v) const;
    void setFloat(const std::string& name, float v) const;
    void setMat4(const std::string& name, const Mat4& mat) const;

private:
    unsigned int programId_;

    static std::string readTextFile(const std::string& path);
    static unsigned int compileShader(unsigned int type, const std::string& src, const std::string& path);
};
