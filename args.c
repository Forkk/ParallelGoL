#include "args.h"

#include <stdlib.h>
#include <getopt.h>
#include <err.h>

struct GlobalOptions opts;

// Valid options
const char* optString = "Ll:rj:";

void setDefaults()
{
	opts.frameLock = false;
	opts.randomize = false;
	opts.threads = 10;
}

bool parseInt(const char* nptr, int* out, int min, int max)
{
	char* endptr;
	int result = strtol(nptr, &endptr, 10);
	// If endptr isn't '\0', the string was probably invalid.
	if (*endptr != '\0') return false;
	else
	{
		// Otherwise, check the value.
		if (result < min || result > max) return false;
		else
		{
			*out = result;
			return true;
		}
	}
}

void processArgs(int argc, char** argv)
{
	setDefaults();
	int opt = 0;
	do
	{
		// errstr for strtonum
		opt = getopt(argc, argv, optString);
		switch (opt)
		{
			// -L enables frame locking
			case 'L':
				opts.frameLock = true;
				break;

			// -r randomizes the scene on startup
			case 'r':
				opts.randomize = true;
				break;

			// -j sets the thread count
			case 'j':
				if (!parseInt(optarg, &opts.threads, 1, 32))
					errx(1, "invalid thread count, must be a number between 1 and 32");
		}
	} while (opt != -1);
}

