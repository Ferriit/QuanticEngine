#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_oldnames.h>
#include <GL/gl.h>
#include <stdbool.h>

struct QEBuffer {
    GLuint VAO, VBO;
};
