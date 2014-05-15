// Header for main.c

#pragma once

#include <stdbool.h>

// {{{ GLUT functions

// Rendering
void renderScene();
void renderCell(int x, int y);
void renderText(int x, int y, const char* text);

// Event handling
void onReshape(int w, int h);
void onMouse(int x, int y);
void onMouseClick(int, int, int x, int y);
void onKeyboard(unsigned char key, int x, int y);

// Misc utility
typedef struct point
{
    float x;
    float y;
} point_t;

point_t screenToGrid(float x, float y);
point_t gridToScreen(float x, float y);

// }}}

