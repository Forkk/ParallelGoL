// Main source file.
// Initializes and starts the program.

#include "grid.h"
#include "grid_updater.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <GL/glut.h>

int screenWidth = 0;
int screenHeight = 0;

// If true, locks the gridSwap function with a mutex while rendering.
// This can prevent small rendering artifacts, but will slow down the
// game if the rendering system can't render a frame faster than the
// update threads.
bool frameLock = true;

void renderScene();
void renderCell(int x, int y);
void onReshape(int w, int h);
void onMouse(int x, int y);
void onMouseClick(int, int, int x, int y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// Initialize the grid.
	initGrid();

	// If we have an argument, load a file from it.
	if (argc > 1)
	{
		char* file = argv[1];
		printf("Loading scene: %s\n", file);
	}
	// Randomize the grid.
	else randomizeGrid();
	

	// Set up GLUT.
	glutInitWindowPosition(0, 0);
	//glutInitWindowSize(1920, 1080);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("Conway's Game of Life");
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(onReshape);
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouse);

	// Run grid updater with 10 threads.
	startGridUpdater(10);

	// Start GLUT.
	glutMainLoop();
}

void renderScene()
{
	// Wait for a tick to finish.
	awaitTick();

	glClear(GL_COLOR_BUFFER_BIT);

	// Lock the grids.
	lockSwapGrids();
	for (int x = 0; x < GRID_W; x++)
		for (int y = 0; y < GRID_H; y++)
			renderCell(x, y);
	unlockSwapGrids();
	
	// Swap buffers
	glutSwapBuffers();
}

float cellw = 2.0f / GRID_W;
float cellh = 2.0f / GRID_H;

void renderCell(int x, int y)
{
	char state = cGrid[x][y];
	if (!state) return;

	float sx = (x * cellw) - 1;
	float sy = (y * cellh) - 1;
	float sx2 = sx + cellw;
	float sy2 = sy + cellh;

	glColor3f(0.0f, 0.0f, 0.0f);
	glRectf(sx, sy, sx2, sy2);
}

void onReshape(int w, int h)
{
	printf("Reshape %i, %i\n", w, h);
	screenWidth = w;
	screenHeight = h;
	glViewport(0, 0, w, h);
}

void onMouseClick(int btn, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		onMouse(x, y);
}

void onMouse(int x, int y)
{
	// Find the cell at the given position.
	// First, divide by width and height to get the position in terms of fractions of the screen.
	// Next, multiply by the cell size.
	float fx = (float)x / (float)screenWidth / cellw * 2;
	float fy = (float)y / (float)screenHeight / cellh * -2;

	// The cell position is 
	printf("Mouse: %f, %f\n", (fx), (fy));
	cGrid[(int)floor(fx-1)][(int)floor(fy  )] = 1;
	cGrid[(int)floor(fx+1)][(int)floor(fy  )] = 1;
	cGrid[(int)floor(fx+1)][(int)floor(fy+1)] = 1;
	cGrid[(int)floor(fx  )][(int)floor(fy+1)] = 1;
	cGrid[(int)floor(fx+1)][(int)floor(fy-1)] = 1;
}

