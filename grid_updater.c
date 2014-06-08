#include "grid_updater.h"

#include "grid.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include <time.h>

// Private declarations

// TODO: Keep a list of threads somewhere.

/// Structure containing settings for an update thread.
struct UpdateThreadParams
{
	/// The minimum x position this thread should update.
	int min;
	/// The maximum x position this thread should update.
	int max;
};

void* gridUpdateThread(void* param);

/// Uses a mutex, conditional, and counter to synchronize threads.
void finishTick();

/// Blocks a thread, waiting for the other threads to finish counting neighbors.
void finishCount();

/// Blocks a thread, waiting for the other threads to finish initializing the next grid.
void finishTickInit();

/// The total number of threads.
int threadCount = 0;

/// A mutex locked counter which counts the number of threads that have finished the current tick.
int threadsDone = 0;
/// If true, the threads will be locked at the end of their tick until set to false.
bool ticksPaused = false;
/// Condition variable for waiting for threads to pause.
pthread_cond_t pauseCond;
pthread_cond_t unpauseCond;

/// Mutex which locks the threadsDone counter.
pthread_mutex_t threadsDoneMutex;
/// A condition variable which controls when the update threads should begin the next tick.
pthread_cond_t threadsDoneCond;

// Same as threadsDone, but for syncing neighbor counting.
int countsDone = 0;
pthread_mutex_t countsDoneMutex;
pthread_cond_t countsDoneCond;

// Same as threadsDone, but for syncing initializing the next grid.
int initsDone = 0;
pthread_mutex_t initsDoneMutex;
pthread_cond_t initsDoneCond;

/// Mutex which protects the swapGrids function.
pthread_mutex_t swapGridsMutex;


/// The Unix time the last tick was finished.
time_t lastTick;

// The number of ticks to average when determining average TPS.
#define TPS_AVERAGE_TICKS 20
/// Array of ticks per second for the last few ticks.
float tpsArray[TPS_AVERAGE_TICKS];
int tpsArrayPos = 0;

/// Average ticks per second over the last 20 frames.
float tpsAverage;

// Code

void startGridUpdater(int threads)
{
	for (int i = 0; i < TPS_AVERAGE_TICKS; i++) tpsArray[i] = 0;
	threadCount = threads;
	// Each thread gets an even portion of the grid.
	int rangeSize = GRID_W / threads;
	for (int i = 0; i < threads; i++)
	{
		struct UpdateThreadParams *params = malloc(sizeof(struct UpdateThreadParams));
		params->min = i * rangeSize;
		params->max = params->min + rangeSize;

		printf("Starting thread %i, rendering %i-%i\n", i, params->min, params->max);

		pthread_attr_t* attrs = malloc(sizeof(pthread_attr_t));
		pthread_attr_init(attrs);
		pthread_t* thread = malloc(sizeof(pthread_t));
		pthread_create(thread, attrs, gridUpdateThread, params);
	}
}

void* gridUpdateThread(void* p)
{
	struct UpdateThreadParams *params = (struct UpdateThreadParams*)p;
	while (1)
	{
		// Update the thread's cells.
		for (int y = 0; y < GRID_H; y++)
		{
			for (int x = params->min; x < params->max; x++)
			{
				updateNeighbors(x, y);
			}
		}

		// Wait for other threads to finish counting neighbors.
		finishCount();

		// Update the life bits.
		for (int y = 0; y < GRID_H; y++)
		{
			for (int x = params->min; x < params->max; x++)
			{
				// If the cell's neighbors haven't changed, ignore it.
				if (nGrid[x][y] & 0b00100000) updateLife(x, y);
			}
		}
		// Synchronize with the other update threads.
		finishTick();

		// Prepare for the next tick.
		// We do this here because we don't want to do it on the first tick.
		for (int y = 0; y < GRID_H; y++)
		{
			for (int x = params->min; x < params->max; x++)
			{
				nGrid[x][y] = cGrid[x][y];
			}
		}

		finishTickInit();
	}
}

void finishTickInit()
{
	pthread_mutex_lock(&initsDoneMutex);

	initsDone++;
	
	if (initsDone >= threadCount)
	{
		// Carry on.
		pthread_cond_broadcast(&initsDoneCond);
		initsDone = 0;
	}
	else
	{
		pthread_cond_wait(&initsDoneCond, &initsDoneMutex);
	}

	pthread_mutex_unlock(&initsDoneMutex);
}

void finishCount()
{
	pthread_mutex_lock(&countsDoneMutex);

	countsDone++;
	
	if (countsDone >= threadCount)
	{
		// Carry on.
		pthread_cond_broadcast(&countsDoneCond);
		countsDone = 0;
	}
	else
	{
		pthread_cond_wait(&countsDoneCond, &countsDoneMutex);
	}

	pthread_mutex_unlock(&countsDoneMutex);
}

void finishTick()
{
	pthread_mutex_lock(&threadsDoneMutex);

	threadsDone++;

	// If all of the threads are done now, begin the next tick.
	if (threadsDone >= threadCount)
	{
		if (ticksPaused)
		{
			pthread_cond_signal(&pauseCond); // Let the main thread know everything's paused.
			pthread_cond_wait(&unpauseCond, &threadsDoneMutex);
		}

		// Unlock the threads.
		pthread_cond_broadcast(&threadsDoneCond);

		// Recalculate the ticks per second.
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
		// TPS = 1000000 / (last tick duration in nanoseconds)
		float cTps = 1000000000 / (float)(now.tv_nsec - lastTick);
		// Add current TPS to the average list.
		tpsArray[tpsArrayPos] = cTps;
		tpsArrayPos++;
		if (tpsArrayPos >= TPS_AVERAGE_TICKS) tpsArrayPos = 0;

		float avgTotal = 0;
		for (int i = 0; i < TPS_AVERAGE_TICKS; i++)
			avgTotal += tpsArray[i];
		tpsAverage = avgTotal / (float)TPS_AVERAGE_TICKS;

		// Update the timestamp.
		lastTick = now.tv_nsec;

		// Reset the counter.
		threadsDone = 0;
		swapGrids();
	}
	// All the threads aren't done yet, wait for them to finish.
	else
	{
		pthread_cond_wait(&threadsDoneCond, &threadsDoneMutex);
	}

	pthread_mutex_unlock(&threadsDoneMutex);
}

void awaitTick()
{
	pthread_mutex_lock(&threadsDoneMutex);
	// Wait for the threads to finish.
	pthread_cond_wait(&threadsDoneCond, &threadsDoneMutex);
	pthread_mutex_unlock(&threadsDoneMutex);
}


void lockTick()
{
	pthread_mutex_lock(&threadsDoneMutex);
	ticksPaused = true;
	// Wait for threads to pause.
	pthread_cond_wait(&pauseCond, &threadsDoneMutex);
	pthread_mutex_unlock(&threadsDoneMutex);
}

void unlockTick()
{
	pthread_mutex_lock(&threadsDoneMutex);
	ticksPaused = false;
	// Unlock the threads.
	pthread_cond_broadcast(&unpauseCond);
	pthread_mutex_unlock(&threadsDoneMutex);
}


void swapGrids()
{
	lockSwapGrids();

	// Swap the dead/alive grids.
	char (*tGrid)[GRID_W][GRID_H] = currentGrid;
	currentGrid = newGrid;
	newGrid = tGrid;

	unlockSwapGrids();
}

void lockSwapGrids()
{
	pthread_mutex_lock(&swapGridsMutex);
}

void unlockSwapGrids()
{
	pthread_mutex_unlock(&swapGridsMutex);
}

