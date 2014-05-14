// Header for loading and saving Game of Life files.

// The format for life files is fairly simple.
// They consist of a grid of ASCII characters where non-whitespace
// characters denote a living cell and spaces or dots (.) denote
// dead cells.
// A newline character denotes the beginning of a new row.
// The width of the file's grid is defined by the width of the
// longest row. Any rows which are too short will be expanded and
// dead cells will be inserted.

#pragma once

struct LifeFile
{
    int gridW;
    int gridH;
    char **grid;
};

/// Tries to read the file at the given path into the given life file struct.
/// Returns nonzero on failure.
int readLifeFile(const char* path, struct LifeFile* dest);


/// Inserts the given life file into the main grid at the given position.
void insertLifeFile(struct LifeFile* lf, int x, int y);

