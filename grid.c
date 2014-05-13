#include "grid.h"

#include <stdlib.h>

void initGrid()
{
	// NOTE: This is never freed.
	// I'm not concerned with it right now, since it's only ever done once...
	currentGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	newGrid = malloc(sizeof(char)*GRID_W*GRID_H);
}

void randomizeGrid()
{
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

