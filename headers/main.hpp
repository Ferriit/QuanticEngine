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

class Engine {
    private:
        SDL_Window* window;
        SDL_GLContext glContext;
        GLuint shaderProgram;

    public:
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

            this->shaderProgram = program;
            return program;
        }

        inline QEModelBuffer createVAO(float* vertices, size_t size) {
            QEModelBuffer buf{};
            glGenVertexArrays(1, &buf.VAO);
            glGenBuffers(1, &buf.VBO);

            glBindVertexArray(buf.VAO);
            glBindBuffer(GL_ARRAY_BUFFER, buf.VBO);
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

            // position
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // color
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            return buf;
        }

        inline int init() {
            // 1. Initialize SDL
            if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
                SDL_Log("SDL_Init Error: %s", SDL_GetError());
                return 1;
            }

            // 2. Set OpenGL attributes
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

            // 3. Create window and context
            this->window = SDL_CreateWindow("Game window", 800, 600, SDL_WINDOW_OPENGL);
            if (!this->window) { SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError()); return 1; }

            this->glContext = SDL_GL_CreateContext(this->window);
            if (!this->glContext) { SDL_Log("SDL_GL_CreateContext Error: %s", SDL_GetError()); return 1; }

            // 4. Initialize GLEW
            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK) {
                SDL_Log("Failed to initialize GLEW");
                return 1;
            }

            const GLubyte* version = glGetString(GL_VERSION);
            if (!version) {
                SDL_Log("Failed to query OpenGL version string");
                return 1;
            }
            SDL_Log("OpenGL Version: %s", version);

            // 5. Dummy OpenGL call to ensure function pointers are loaded
            glGetError();
            glGetString(GL_VERSION); 

            // 6. Create shaders and use program
            this->shaderProgram = createShaderProgram("shaders/vert.glsl", "shaders/frag.glsl");
            //glUseProgram(createShaderProgram("shaders/vert.glsl", "shaders/frag.glsl"));

            // 7. Optional: VSync
            //SDL_GL_SetSwapInterval(1);
        }

        inline int update(float* vertices, size_t vertex_size) {
            QEModelBuffer triangle = createVAO(vertices, vertex_size);

            // --- Main loop ---
            bool running = true;
            SDL_Event event;
            while (running) {
                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_EVENT_QUIT:
                            running = false;
                            break;
                        case SDL_EVENT_KEY_DOWN:
                            if (event.key.key == SDLK_ESCAPE)
                                running = false;
                            break;
                    }
                }

                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glBindVertexArray(triangle.VAO);
                glDrawArrays(GL_TRIANGLES, 0, 3);

                SDL_GL_SwapWindow(window);
            }
            return 0;
        }

        inline int cleanup() {
            SDL_GL_DestroyContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 0;
        }
};
