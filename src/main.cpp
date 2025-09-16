#include "../headers/main.hpp"

float vertices[] = {
    // positions        // colors
     0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // top, red
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left, green
     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // bottom right, blue
};

QEModelBuffer createVAO(float* vertices, size_t size) {
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

int main() {
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window *window = SDL_CreateWindow("Game window", 800, 600, SDL_WINDOW_OPENGL);

    if (!window) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // --- Create OpenGL context ---
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        SDL_Log("SDL_GL_CreateContext Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW");
        return 1;
    }

    // Enable VSync (optional)
    SDL_GL_SetSwapInterval(1);

    // Setup triangle + shader
    QEModelBuffer triangle = createVAO(vertices, sizeof(vertices));
    GLuint shaderProgram = createShaderProgram("shaders/vert.glsl", "shaders/frag.glsl");

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

        glUseProgram(shaderProgram);
        glBindVertexArray(triangle.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
