#include "grid.h"

#include <stdlib.h>

void initGrid()
{
	// NOTE: This is never freed.
	// I'm not concerned with it right now, since it's only ever done once...
	currentGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	newGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	cActiveGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	nActiveGrid = malloc(sizeof(char)*GRID_W*GRID_H);

	// Activate all the cells.
	for (int x = 0; x < GRID_W; x++)
		for (int y = 0; y < GRID_H; y++)
			cActGrid[x][y] = 1;
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
	// Skip inactive cells
	if (!cActGrid[x][y]) return;

	char state = cGrid[x][y];

	int u = y-1;
	int d = y+1;
	int l = x-1;
	int r = x+1;

	if (u <  0     ) u = GRID_H-1;
	if (d >= GRID_H) d = 0;
	if (l <  0     ) l = GRID_W-1;
	if (r >= GRID_W) r = 0;

	int neighbors = 0;
	// Count surrounding cells.
	if (cGrid[x][u]) neighbors++;
	if (cGrid[x][d]) neighbors++;
	if (cGrid[l][y]) neighbors++;
	if (cGrid[r][y]) neighbors++;

	if (cGrid[l][u]) neighbors++;
	if (cGrid[r][d]) neighbors++;
	if (cGrid[l][d]) neighbors++;
	if (cGrid[r][u]) neighbors++;

	// Apply the rules.
	char newState = 0;
	if (state)
	{
		// Any live cell with fewer than two live neighbours dies, as if caused by under-population.
		// Any live cell with more than three live neighbours dies, as if by overcrowding.
		if (neighbors < 2 || neighbors > 3)
			newState = 0;
		// Any live cell with two or three live neighbours lives on to the next generation.
		else
			newState = state;
	}
	// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
	else if (neighbors == 3)
		newState = 1;
	else
		newState = state;

	nGrid[x][y] = newState;
	if (state != newState)
	{
		// Activate the cell and its neighbors.
		nActGrid[x][y] = 1;

		nActGrid[x][u] = 1;
		nActGrid[x][d] = 1;
		nActGrid[l][y] = 1;
		nActGrid[r][y] = 1;

		nActGrid[l][u] = 1;
		nActGrid[l][d] = 1;
		nActGrid[r][u] = 1;
		nActGrid[r][d] = 1;
	}
}

