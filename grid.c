#include "grid.h"

#include <stdlib.h>

void initGrid()
{
	// NOTE: This is never freed.
	// I'm not concerned with it right now, since it's only ever done once...
	currentGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	newGrid = malloc(sizeof(char)*GRID_W*GRID_H);

	// Clear
	for (int x = 0; x < GRID_W; x++)
	{
		for (int y = 0; y < GRID_H; y++)
		{
			cGrid[x][y] = 0;
			nGrid[x][y] = 0;
		}
	}
}

void randomizeGrid()
{
	for (int x = 0; x < GRID_W; x++)
	{
		for (int y = 0; y < GRID_H; y++)
		{
			char val = 0;
			val = rand() % 2;
			if (val)
			{
				cGrid[x][y] = 0b11000000;
				nGrid[x][y] = 0b11000000;
			}
			else
			{
				cGrid[x][y] = 0b00000000;
				nGrid[x][y] = 0b00000000;
			}
		}
	}
}

void updateNeighbors(int x, int y)
{
	char state = cGrid[x][y];

	int u = y-1;
	int d = y+1;
	int l = x-1;
	int r = x+1;

	if (u <  0     ) u = GRID_H-1;
	if (d >= GRID_H) d = 0;
	if (l <  0     ) l = GRID_W-1;
	if (r >= GRID_W) r = 0;

	// If the cell changed last tick.
	if (state & 0b01000000)
	{
		int increment = -1;
		if (state & 0b10000000) increment = 1;

		// Add one to neighbors and set them to active.
		nGrid[x][u] = (nGrid[x][u] + increment) | 0b00100000;
		nGrid[x][d] = (nGrid[x][d] + increment) | 0b00100000;
		nGrid[l][y] = (nGrid[l][y] + increment) | 0b00100000;
		nGrid[r][y] = (nGrid[r][y] + increment) | 0b00100000;

		nGrid[l][u] = (nGrid[l][u] + increment) | 0b00100000;
		nGrid[l][d] = (nGrid[l][d] + increment) | 0b00100000;
		nGrid[r][u] = (nGrid[r][u] + increment) | 0b00100000;
		nGrid[r][d] = (nGrid[r][d] + increment) | 0b00100000;
	}
}

void updateLife(int x, int y)
{
	char state = nGrid[x][y];

	// Check the cell's neighbor count.
	char neighbors = state & 0b00001111;

	// Find neighbors.
	int u = y-1;
	int d = y+1;
	int l = x-1;
	int r = x+1;

	if (u <  0     ) u = GRID_H-1;
	if (d >= GRID_H) d = 0;
	if (l <  0     ) l = GRID_W-1;
	if (r >= GRID_W) r = 0;

	char newState = state;

	// Apply the rules.
	// Live cells
	if (newState & 0b10000000)
	{
		// Any live cell with fewer than two live neighbours dies, as if caused by under-population.
		// Any live cell with more than three live neighbours dies, as if by overcrowding.
		if (neighbors < 2 || neighbors > 3)
		{
			// The state changed, kill it and set the change bit.
			newState = (newState & 0b01111111) | 0b01000000;
		}
		// Any live cell with two or three live neighbours lives on to the next generation.
		else
		{
			newState = (newState | 0b10000000) & 0b10111111;
		}
	}
	// Dead cells
	else
	{
		// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
		if (neighbors == 3)
		{
			// The state changed, revive it and set the change bit.
			newState = newState | 0b11000000;
		}
		else
		{
			newState &= 0b00111111;
		}
	}

	nGrid[x][y] = newState;
}

