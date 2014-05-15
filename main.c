// Main source file.
// Initializes and starts the program.

#include "main.h"

#include "args.h"
#include "grid.h"
#include "grid_updater.h"
#include "life_file.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <GL/glut.h>

int screenWidth = 0;
int screenHeight = 0;

struct LifeFile lf;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// Process arguments.
	processArgs(argc, argv);

	if (opts.frameLock)
	printf("Frame lock is on.\n");

	// Initialize the grid.
	initGrid();

	// If we have an argument, load a file from it.
//	if (argc > 1)
//	{
//		char* file = argv[1];
//		printf("Loading scene: %s\n", file);
//		readLifeFile(file, &lf);
//	}
	// Randomize the grid.
	if (opts.randomize) randomizeGrid();
	

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
	glutKeyboardFunc(onKeyboard);

	printf("Running updater with %i threads.\n", opts.threads);

	// Run the grid updater.
	startGridUpdater(opts.threads);

	// Start GLUT.
	glutMainLoop();
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screenWidth, 0, screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	// Lock the grids.
	if (opts.frameLock) lockSwapGrids();
	for (int x = 0; x < GRID_W; x++)
		for (int y = 0; y < GRID_H; y++)
			renderCell(x, y);
	if (opts.frameLock) unlockSwapGrids();
	
	// Draw some text.
	renderText(8, screenHeight - 24, "Testing");

	// Swap buffers
	glutSwapBuffers();
}

// Renders the given null terminated string at the given position.
void renderText(int x, int y, const char* text)
{
	glPushMatrix();

	float fx = (float)x;
	float fy = (float)y;

	glTranslatef(fx, fy, 0.0);
	glScalef(0.1, 0.1, 0.1);
	
	for (int i = 0; text[i] != '\0'; i++)
	{
		glTranslatef(32, 0.0, 0.0);
		glColor3f(0.8, 0.0, 0.0);
		int ch = text[i];
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, ch);
	}

	glPopMatrix();
}

float cellw = 0;
float cellh = 0;

void renderCell(int x, int y)
{
	char state = cGrid[x][y];
	if (!(state & 0b10000000)) return;

	// Get the screen coordinates for the cell.
	point_t pos = gridToScreen(x, y);

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex2f(pos.x        , pos.y        );
		glVertex2f(pos.x        , pos.y + cellh);
		glVertex2f(pos.x + cellw, pos.y + cellh);
		glVertex2f(pos.x + cellw, pos.y        );
	glEnd();
}

void onReshape(int w, int h)
{
	screenWidth = w;
	screenHeight = h;

	cellw = (float)w / GRID_W;
	cellh = (float)h / GRID_H;
	printf("Screen size: %d, %d - Cell size: %f, %f\n", w, h, cellw, cellh);
}

void onMouseClick(int btn, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		onMouse(x, y);
}

void onMouse(int x, int y)
{
	// Find the cell at the given position.
	point_t cell = screenToGrid(x, y);

	lockTick();
	nGrid[(int)floor(cell.x-1)][(int)floor(cell.y  )] |= 0b11000000;
	nGrid[(int)floor(cell.x+1)][(int)floor(cell.y  )] |= 0b11000000;
	nGrid[(int)floor(cell.x+1)][(int)floor(cell.y+1)] |= 0b11000000;
	nGrid[(int)floor(cell.x  )][(int)floor(cell.y+1)] |= 0b11000000;
	nGrid[(int)floor(cell.x+1)][(int)floor(cell.y-1)] |= 0b11000000;
	unlockTick();
	//printf("Place at %d, %d\n", (int)floor(cell.x), (int)floor(cell.y));

	//insertLifeFile(&lf, cell.x, cell.y);
}

void onKeyboard(unsigned char key, int x, int y)
{
	// Exit on escape.
	switch (key)
	{
		case 27:
			exit(0);
			break;

		default:
			break;
	}
}

point_t screenToGrid(float x, float y)
{
	point_t pos;
	pos.x = x / cellw;
	pos.y = screenHeight + y / cellh;
	return pos;
}

point_t gridToScreen(float x, float y)
{
	point_t pos;
	pos.x = x * cellw;
	pos.y = screenHeight - y * cellh;
	return pos;
}

