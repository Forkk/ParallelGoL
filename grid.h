// Header for the grid system.
// Controls the grid and updating.

#pragma once

#define GRID_W 1000
#define GRID_H 500

// We have two grids, the current grid (which is the grid for the current frame) and the new grid.
// Each grid alternates being the current grid and new grid.
// The data in each cell is as follows:
// The first bit indicates whether the cell is alive or dead.
// The second bit indicates whether the cell is active or not (currently not used).
// The third bit contains the cell's previous life state.
// The rest of the bits indicate how many neighbors the cell has.
char (*currentGrid)[GRID_W][GRID_H];
char (*newGrid)[GRID_W][GRID_H];


// Macros for nice and easy access to grid pointers.
#define cGrid (*currentGrid)
#define nGrid (*newGrid)


/// Initializes the grid, allocating its memory
void initGrid();

/// Randomizes the cells in the grid.
void randomizeGrid();

/**
 * Updates the neighbor counts of the cells around the given position and
 * sets the life bit for the given position.
 */
void updateNeighbors(int x, int y);

/**
 * Updates the given cell's life bit based on its neighbor count.
 */
void updateLife(int x, int y);

