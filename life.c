#include <GL/glut.h>
#include <pthread.h>
#include <stdio.h>

#define GRID_W 1000
#define GRID_H 500

// We have two grids, the current grid (which is the grid for the current frame) and the new grid.
// Each grid alternates being the current grid and new grid.
char (*currentGrid)[GRID_W][GRID_H];
char (*newGrid)[GRID_W][GRID_H];

// Macros for nice and easy access to grid pointers.
#define cGrid (*currentGrid)
#define nGrid (*newGrid)

// Counter for the number of threads that have finished their work for the current frame.
// When this is equal to UPDATE_THREADS + RENDER_THREADS, the next frame begins.
int updatesDone = 0;
pthread_mutex_t updateCountMutex;
pthread_cond_t updateCond;

void mainLoop();
void updateCell(int x, int y);
void renderCell(int x, int y);
void* updateThread(void* arg);
void renderScene();
void initGrid();
void finishFrame(); // Should be called when a thread finishes its current frame. Handles synchronization.
void awaitFinishFrame(); // Waits for a frame to finish. Does not increment the update count.

// Structure which defines a range of x positions a given thread will be responsible for updating.
struct UpdateThreadRange
{
	int min;
	int max;
};

#define UPDATE_THREADS 10
#define RENDER_THREADS 1

int main(int argc, char** argv)
{
	// Create the grid.
	initGrid();

	// Initialize the update threads.
	struct UpdateThreadRange* updateRanges = malloc(sizeof(struct UpdateThreadRange)*UPDATE_THREADS);
	pthread_t threads[UPDATE_THREADS];
	pthread_attr_t attrs[UPDATE_THREADS];
	int rangeSize = GRID_W / UPDATE_THREADS;
	for (int i = 0; i < UPDATE_THREADS; i++)
	{
		updateRanges[i].min = rangeSize*i;
		updateRanges[i].max = rangeSize*(i+1);
		// Prevent overlap
		//if (i-1 < UPDATE_THREADS) updateRanges[i].max--;

		pthread_attr_init(&attrs[i]);
		pthread_create(&threads[i], &attrs[i], updateThread, &updateRanges[i]);
	}

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("Conway's Game of Life");
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);

	glutMainLoop();
}

void initGrid()
{
	// Allocate.
	currentGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	newGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	for (int x = 0; x < GRID_W; x++)
	{
		for (int y = 0; y < GRID_H; y++)
		{
			char val = 0;
			val = rand() % 2;
			cGrid[x][y] = val;
			nGrid[x][y] = val;
		}
	}
}

void renderScene()
{
	// Wait for an update to finish.
	awaitFinishFrame();

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


void finishFrame()
{
	// Lock the mutex.
	pthread_mutex_lock(&updateCountMutex);

	// Increment the counter.
	updatesDone++;

	// If all threads are done, signal.
	if (updatesDone >= UPDATE_THREADS)
	{
		pthread_cond_broadcast(&updateCond);
		updatesDone = 0;

		// Swap grids
		char (*tGrid)[GRID_W][GRID_H] = currentGrid;
		currentGrid = newGrid;
		newGrid = tGrid;
	}
	// Otherwise, wait for the rest of the threads to finish.
	else
	{
		pthread_cond_wait(&updateCond, &updateCountMutex);
	}

	// Unlock the mutex.
	pthread_mutex_unlock(&updateCountMutex);
}

void awaitFinishFrame()
{
	// Lock the mutex.
	pthread_mutex_lock(&updateCountMutex);

	// Wait for the threads to finish.
	pthread_cond_wait(&updateCond, &updateCountMutex);

	// Unlock the mutex.
	pthread_mutex_unlock(&updateCountMutex);
}


//////////////////////////////
// Threading
//
// Multithreading is done by having several separate "updater" threads responsible for updating certain ranges of the grid.
//

// Entry point for an updater thread.
void* updateThread(void* arg)
{
	struct UpdateThreadRange range = *((struct UpdateThreadRange*)arg);
	while (1)
	{
		// Update our cells.
		for (int x = range.min; x < range.max; x++)
			for (int y = 0; y < GRID_H; y++)
				updateCell(x, y);

		// Wait for other threads to finish.
		finishFrame();
	}
}

void updateCell(int x, int y)
{
	char state = cGrid[x][y];

	int neighbors = 0;
	// Count surrounding cells.
	if (y > 0       && cGrid[x][y-1]) neighbors++;
	if (y < GRID_H  && cGrid[x][y+1]) neighbors++;
	if (x > 0       && cGrid[x-1][y]) neighbors++;
	if (x < GRID_W  && cGrid[x+1][y]) neighbors++;

	if (x > 0       && y > 0       && cGrid[x-1][y-1]) neighbors++;
	if (x < GRID_W  && y < GRID_H  && cGrid[x+1][y+1]) neighbors++;
	if (x > 0       && y < GRID_H  && cGrid[x-1][y+1]) neighbors++;
	if (x < GRID_W  && y > 0       && cGrid[x+1][y-1]) neighbors++;

	// Apply the rules.
	if (state)
	{
		// Any live cell with fewer than two live neighbours dies, as if caused by under-population.
		// Any live cell with more than three live neighbours dies, as if by overcrowding.
		if (neighbors < 2 || neighbors > 3)
			nGrid[x][y] = 0;
		// Any live cell with two or three live neighbours lives on to the next generation.
		else
			nGrid[x][y] = state;
	}
	// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
	else if (neighbors == 3)
		nGrid[x][y] = 1;
	else
		nGrid[x][y] = state;
}

