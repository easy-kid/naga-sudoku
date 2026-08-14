#ifndef SUDOKU_GENERATOR_H
#define SUDOKU_GENERATOR_H

#include "sudoku_core.h"

// Solves grid in-place using backtracking. Returns true if solved.
bool sudoku_solver_solve(int grid[9][9]);

// Counts solutions up to max_count (useful to check uniqueness where max_count = 2)
int sudoku_solver_count_solutions(int grid[9][9], int max_count);

// Generates a new valid Sudoku puzzle for the specified difficulty
void sudoku_generator_generate(SudokuBoard* board, Difficulty diff);

#endif // SUDOKU_GENERATOR_H
