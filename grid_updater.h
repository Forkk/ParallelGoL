// Header for the grid update system.
// This system is responsible for running, managing, and synchronizing the grid's updater threads.

#pragma once

/// Spawns the given number of threads to update the grid.
void startGridUpdater(int threadCount);

/// Blocks until a tick is finished.
void awaitTick();

/// Swaps the new and current grids to prepare for the next tick.
/// This is mutex-protected.
void swapGrids();

/// Locks the swapGrids mutex, preventing the grids from swapping.
void lockSwapGrids();

/// Unlocks the swapGrids mutex, preventing the grids from swapping.
void unlockSwapGrids();

