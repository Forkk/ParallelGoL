#include "grid.h"

#include <stdlib.h>

void initGrid()
{
	// NOTE: This is never freed.
	// I'm not concerned with it right now, since it's only ever done once...
	currentGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	newGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	//cActiveGrid = malloc(sizeof(char)*GRID_W*GRID_H);
	//nActiveGrid = malloc(sizeof(char)*GRID_W*GRID_H);

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
				cGrid[x][y] |= 0b10000000;
				nGrid[x][y] |= 0b10000000;
			}
			cGrid[x][y] |= 0b01000000;
			nGrid[x][y] |= 0b01000000;
		}
	}
}

void updateNeighbors(int x, int y)
{
	char state = cGrid[x][y];

	// If the cell is alive
	if (state & 0b10000000)
	{
		// Set the life bit.
		nGrid[x][y] |= 0b10000000;

		int u = y-1;
		int d = y+1;
		int l = x-1;
		int r = x+1;

		if (u <  0     ) u = GRID_H-1;
		if (d >= GRID_H) d = 0;
		if (l <  0     ) l = GRID_W-1;
		if (r >= GRID_W) r = 0;

		// Add one to neighbors and set them to active.
		nGrid[x][u] += 1;
		nGrid[x][d] += 1;
		nGrid[l][y] += 1;
		nGrid[r][y] += 1;

		nGrid[l][u] += 1;
		nGrid[l][d] += 1;
		nGrid[r][u] += 1;
		nGrid[r][d] += 1;
	}
	// Otherwise, unset life bit.
	else nGrid[x][y] &= 0b01111111;
}

void updateLife(int x, int y)
{
	// Check the cell's neighbor count.
	char neighbors = nGrid[x][y] & 0b00001111;

	// Find neighbors.
	int u = y-1;
	int d = y+1;
	int l = x-1;
	int r = x+1;

	if (u <  0     ) u = GRID_H-1;
	if (d >= GRID_H) d = 0;
	if (l <  0     ) l = GRID_W-1;
	if (r >= GRID_W) r = 0;

	// Apply the rules.
	// Live cells
	if (nGrid[x][y] & 0b10000000)
	{
		// Any live cell with fewer than two live neighbours dies, as if caused by under-population.
		// Any live cell with more than three live neighbours dies, as if by overcrowding.
		if (neighbors < 2 || neighbors > 3)
		{
			nGrid[x][y] &= 0b01111111;
			
			// The state changed, set the active bit.
//			nGrid[x][u] |= 0b01000000;
//			nGrid[x][d] |= 0b01000000;
//			nGrid[l][y] |= 0b01000000;
//			nGrid[r][y] |= 0b01000000;
//
//			nGrid[l][u] |= 0b01000000;
//			nGrid[l][d] |= 0b01000000;
//			nGrid[r][u] |= 0b01000000;
//			nGrid[r][d] |= 0b01000000;
		}
		// Any live cell with two or three live neighbours lives on to the next generation.
		else
		{
			nGrid[x][y] |= 0b10000000;
		}
	}
	// Dead cells
	else
	{
		// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
		if (neighbors == 3)
		{
			nGrid[x][y] |= 0b10000000;

			// The state changed, set the active bit.
//			nGrid[x][u] |= 0b01000000;
//			nGrid[x][d] |= 0b01000000;
//			nGrid[l][y] |= 0b01000000;
//			nGrid[r][y] |= 0b01000000;
//
//			nGrid[l][u] |= 0b01000000;
//			nGrid[l][d] |= 0b01000000;
//			nGrid[r][u] |= 0b01000000;
//			nGrid[r][d] |= 0b01000000;
		}
		else
		{
			// Stay dead.
			nGrid[x][y] &= 0b01111111;
		}
	}
}

