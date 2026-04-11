#include "core/Shader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include <glad/glad.h>

namespace {
std::string readLog(unsigned int object, bool isProgram) {
    int logLength = 0;
    if (isProgram) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logLength);
    } else {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
    }

    if (logLength <= 1) {
        return std::string();
    }

    std::string log(static_cast<size_t>(logLength), '\0');
    if (isProgram) {
        glGetProgramInfoLog(object, logLength, nullptr, log.data());
    } else {
        glGetShaderInfoLog(object, logLength, nullptr, log.data());
    }
    return log;
}
}

Shader::Shader(const std::string& vertPath, const std::string& fragPath)
    : programId_(0) {
    const std::string vertSrc = readTextFile(vertPath);
    const std::string fragSrc = readTextFile(fragPath);

    const unsigned int vs = compileShader(GL_VERTEX_SHADER, vertSrc, vertPath);
    const unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragSrc, fragPath);

    programId_ = glCreateProgram();
    glAttachShader(programId_, vs);
    glAttachShader(programId_, fs);
    glLinkProgram(programId_);

    int success = 0;
    glGetProgramiv(programId_, GL_LINK_STATUS, &success);
    if (success == 0) {
        const std::string log = readLog(programId_, true);
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(programId_);
        programId_ = 0;
        throw std::runtime_error("Program link failed: " + log);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader() {
    if (programId_ != 0) {
        glDeleteProgram(programId_);
    }
}

Shader::Shader(Shader&& other) noexcept
    : programId_(other.programId_) {
    other.programId_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    if (programId_ != 0) {
        glDeleteProgram(programId_);
    }

    programId_ = other.programId_;
    other.programId_ = 0;
    return *this;
}

void Shader::use() const {
    glUseProgram(programId_);
}

void Shader::setInt(const std::string& name, int v) const {
    const int loc = glGetUniformLocation(programId_, name.c_str());
    glUniform1i(loc, v);
}

void Shader::setFloat(const std::string& name, float v) const {
    const int loc = glGetUniformLocation(programId_, name.c_str());
    glUniform1f(loc, v);
}

void Shader::setMat4(const std::string& name, const Mat4& mat) const {
    const int loc = glGetUniformLocation(programId_, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat.m);
}

std::string Shader::readTextFile(const std::string& path) {
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(unsigned int type, const std::string& src, const std::string& path) {
    const unsigned int shader = glCreateShader(type);
    const char* ptr = src.c_str();
    glShaderSource(shader, 1, &ptr, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == 0) {
        const std::string log = readLog(shader, false);
        glDeleteShader(shader);
        throw std::runtime_error("Shader compile failed [" + path + "]: " + log);
    }

    return shader;
}
