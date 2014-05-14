// Header for the command line argument system.

#pragma once

#include <stdbool.h>

/**
 * Processes command line arguments, setting fields in the GlobalOptions struct accordingly.
 * Prints usage and exits with a nonzero error code on failure.
 */
void processArgs(int argc, char** argv);

/**
 * Loads the default set of options into the global options struct.
 */
void setDefaults();

struct GlobalOptions
{
    /**
     * Pauses the grid update threads while rendering.
     * This will slow down the game, but can help reduce visual artifacts.
     * Default: false
     * Argument: -L
     */
    bool frameLock;

    /**
     * If set, randomizes the grid on startup.
     * Default: false
     * Argument: -r
     */
    bool randomize;

    /**
     * The number of updater threads to use.
     * Default: 10
     * Argument: -j
     */
    int threads;
};

extern struct GlobalOptions opts;

