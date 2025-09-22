#include "../headers/main.hpp"

float vertices[] = {
    // positions        // colors
     0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // top, red
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left, green
     0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // bottom right, blue
};

int main() {
    Engine engine;
    engine.init();

    engine.update(vertices, sizeof(vertices));

    engine.cleanup();
}
