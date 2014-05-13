#include "grid_updater.h"

#include "grid.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

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

/// Mutex which protects the swapGrids function.
pthread_mutex_t swapGridsMutex;

// Code

void startGridUpdater(int threads)
{
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
		for (int x = params->min; x < params->max; x++)
			for (int y = 0; y < GRID_H; y++)
				updateCell(x, y);
		// Synchronize with the other update threads.
		finishTick();
	}
}


// Internal function. Unlocks the threads at the end of the tick.
void unlockTickEnd()
{
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

