#include "grid_updater.h"

#include "grid.h"

#include <stdlib.h>
#include <stdio.h>
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
/// Mutex which locks the threadsDone counter.
pthread_mutex_t threadsDoneMutex;
/// A condition variable which controls when the update threads should begin the next tick.
pthread_cond_t threadsDoneCond;

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

void finishTick()
{
	// First, we need to lock the mutex.
	pthread_mutex_lock(&threadsDoneMutex);

	// Next, we increment the counter.
	threadsDone++;

	// If all of the threads are done now, begin the next tick.
	if (threadsDone >= threadCount)
	{
		// First, broadcast that the tick is finished.
		pthread_cond_broadcast(&threadsDoneCond);
		// Reset the counter.
		threadsDone = 0;
		
		// Finally, swap the grids.
		char (*tGrid)[GRID_W][GRID_H] = currentGrid;
		currentGrid = newGrid;
		newGrid = tGrid;
	}
	// All the threads aren't done yet, wait for them to finish.
	else
	{
		pthread_cond_wait(&threadsDoneCond, &threadsDoneMutex);
	}

	// Be sure to unlock the mutex.
	pthread_mutex_unlock(&threadsDoneMutex);
}

void awaitTick()
{
	// Lock the mutex.
	pthread_mutex_lock(&threadsDoneMutex);
	
	// Wait for the threads to finish.
	pthread_cond_wait(&threadsDoneCond, &threadsDoneMutex);

	// Unlock the mutex.
	pthread_mutex_unlock(&threadsDoneMutex);
}

