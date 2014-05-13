// Main source file.
// Initializes and starts the program.

#include "grid.h"
#include "grid_updater.h"

#include <stdlib.h>
#include <GL/glut.h>

void renderScene();
void renderCell(int x, int y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// Initialize the grid.
	initGrid();
	
	// Randomize the grid.
	randomizeGrid();

	// Set up GLUT.
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("Conway's Game of Life");
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

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

	for (int x = 0; x < GRID_W; x++)
	{
		for (int y = 0; y < GRID_H; y++)
		{
			renderCell(x, y);
		}
	}
	
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

