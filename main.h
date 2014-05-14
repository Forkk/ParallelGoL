// Header for main.c

#pragma once

#include <stdbool.h>

// {{{ GLUT functions

// Rendering
void renderScene();
void renderCell(int x, int y);

// Event handling
void onReshape(int w, int h);
void onMouse(int x, int y);
void onMouseClick(int, int, int x, int y);

// }}}

