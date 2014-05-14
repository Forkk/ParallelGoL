// Header for the grid system.
// Controls the grid and updating.

#pragma once

#define GRID_W 2000
#define GRID_H 1000

// We have two grids, the current grid (which is the grid for the current frame) and the new grid.
// Each grid alternates being the current grid and new grid.
char (*currentGrid)[GRID_W][GRID_H];
char (*newGrid)[GRID_W][GRID_H];

// For optimization, we have a second pair of grids which keep track of active cells.
// If a cell's state changes, all of its neighbors are marked as active.
char (*cActiveGrid)[GRID_W][GRID_H];
char (*nActiveGrid)[GRID_W][GRID_H];


// Macros for nice and easy access to grid pointers.
#define cGrid (*currentGrid)
#define nGrid (*newGrid)

#define cActGrid (*cActiveGrid)
#define nActGrid (*nActiveGrid)


/// Initializes the grid, allocating its memory
void initGrid();

/// Randomizes the cells in the grid.
void randomizeGrid();

/// Updates the cell in the grid at the given position.
void updateCell(int x, int y);

