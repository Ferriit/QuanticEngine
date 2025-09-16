#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_oldnames.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <stdbool.h>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>


struct QEModelBuffer {
    GLuint VAO, VBO;
};

// --- Shader sources ---
static const char* vertexShaderSource = R"(
)";

static const char* fragmentShaderSource = R"(
)";

inline std::string loadFileAsString(const char* path) {
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open file: ") + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline GLuint compileShaderFromFile(GLenum type, const char* path) {
    std::string code = loadFileAsString(path);
    const char* source = code.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Shader compilation failed (") + path + "): " + infoLog);
    }

    return shader;
}

inline GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    GLuint vertexShader = compileShaderFromFile(GL_VERTEX_SHADER, vertexPath);
    GLuint fragmentShader = compileShaderFromFile(GL_FRAGMENT_SHADER, fragmentPath);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Program linking failed: ") + infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
