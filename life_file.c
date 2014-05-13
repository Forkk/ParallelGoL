#include "life_file.h"

#include "grid.h"
#include "grid_updater.h"

#include <stdlib.h>
#include <stdio.h>

int readLifeFile(const char* path, struct LifeFile* dest)
{
    FILE* file = fopen(path, "r");
    if (!file) return -1;

    // TODO: There's probably a better way to do this without reading the file twice.
    
    // Read through the file to determine the dimensions of the grid.
    int rowCount = 0;
    int colCount = 0;
    int maxCols = 0; // Max number of cols in any row.
    while (1)
    {
        char cellChar;
        int r = fscanf(file, "%c", &cellChar);
        if (r == EOF || cellChar == '\n')
        {
            // Next row
            rowCount++;
            // If this row was longer than any other rows, record the nex maximum column length.
            if (maxCols < colCount) maxCols = colCount;
            colCount = 0;

            // If we hit EOF, stop
            if (r == EOF) break;
        }
        // Increment the column count.
        else colCount++;
    }
    colCount = maxCols;

    // Rewind the file stream.
    rewind(file);

    // Now, we need to allocate the arrays for the grid.
    char **grid = malloc(colCount * sizeof(char*));
    for (int i = 0; i < colCount; i++)
    {
        grid[i] = calloc(rowCount, 0);
    }

    // Read the file again
    int x = 0;
    int y = 0;
    while (1)
    {
        char cellChar;
        // Read the current byte.
        if (fscanf(file, "%c", &cellChar) == EOF) break;

        if (cellChar == '\n')
        {
            // If we hit a newline, reset x and increment y.
            x = 0;
            y++;
            printf("%i\n", y);
            continue;
        }
        else if (cellChar == ' ' || cellChar == '.')
            grid[x][y] = 0;
        else
            grid[x][y] = 1;

        // Increment x
        x++;
    }

    // Load the info into the data structure.
    dest->gridW = colCount;
    dest->gridH = rowCount;
    dest->grid = grid;

    printf("Loaded %dx%d life file.\n", colCount, rowCount);

    return 0;
}


void insertLifeFile(struct LifeFile* file, int x, int y)
{
    lockTick();
    for (int lx = 0; lx < file->gridW; lx++)
    {
        for (int ly = 0; ly < file->gridH; ly++)
        {
            cGrid[x+lx][y+ly] = file->grid[lx][ly];
            nGrid[x+lx][y+ly] = file->grid[lx][ly];
        }
    }
    unlockTick();
}

