// Header for the grid system.
// Controls the grid and updating.

#pragma once

#define GRID_W 1000
#define GRID_H 500

// We have two grids, the current grid (which is the grid for the current frame) and the new grid.
// Each grid alternates being the current grid and new grid.
char (*currentGrid)[GRID_W][GRID_H];
char (*newGrid)[GRID_W][GRID_H];

// Macros for nice and easy access to grid pointers.
#define cGrid (*currentGrid)
#define nGrid (*newGrid)

/// Initializes the grid, allocating its memory
void initGrid();

/// Randomizes the cells in the grid.
void randomizeGrid();

/// Updates the cell in the grid at the given position.
void updateCell(int x, int y);

