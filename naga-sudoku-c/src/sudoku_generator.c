#include "sudoku_generator.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool is_safe_in_grid(const int grid[9][9], int row, int col, int num) {
    for (int c = 0; c < 9; c++) {
        if (grid[row][c] == num) return false;
    }
    for (int r = 0; r < 9; r++) {
        if (grid[r][col] == num) return false;
    }
    int start_r = (row / 3) * 3;
    int start_c = (col / 3) * 3;
    for (int r = start_r; r < start_r + 3; r++) {
        for (int c = start_c; c < start_c + 3; c++) {
            if (grid[r][c] == num) return false;
        }
    }
    return true;
}

static void shuffle_array(int* arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

static bool solve_backtrack_randomized(int grid[9][9]) {
    int row = -1, col = -1;
    bool empty_found = false;
    
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (grid[r][c] == 0) {
                row = r;
                col = c;
                empty_found = true;
                break;
            }
        }
        if (empty_found) break;
    }
    
    if (!empty_found) return true; // Solved
    
    int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle_array(nums, 9);
    
    for (int i = 0; i < 9; i++) {
        int val = nums[i];
        if (is_safe_in_grid(grid, row, col, val)) {
            grid[row][col] = val;
            if (solve_backtrack_randomized(grid)) {
                return true;
            }
            grid[row][col] = 0;
        }
    }
    return false;
}

bool sudoku_solver_solve(int grid[9][9]) {
    int row = -1, col = -1;
    bool empty_found = false;
    
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (grid[r][c] == 0) {
                row = r;
                col = c;
                empty_found = true;
                break;
            }
        }
        if (empty_found) break;
    }
    
    if (!empty_found) return true;
    
    for (int val = 1; val <= 9; val++) {
        if (is_safe_in_grid(grid, row, col, val)) {
            grid[row][col] = val;
            if (sudoku_solver_solve(grid)) {
                return true;
            }
            grid[row][col] = 0;
        }
    }
    return false;
}

static void count_solutions_recurse(int grid[9][9], int* count, int max_count) {
    if (*count >= max_count) return;
    
    int row = -1, col = -1;
    bool empty_found = false;
    
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (grid[r][c] == 0) {
                row = r;
                col = c;
                empty_found = true;
                break;
            }
        }
        if (empty_found) break;
    }
    
    if (!empty_found) {
        (*count)++;
        return;
    }
    
    for (int val = 1; val <= 9; val++) {
        if (is_safe_in_grid(grid, row, col, val)) {
            grid[row][col] = val;
            count_solutions_recurse(grid, count, max_count);
            grid[row][col] = 0;
            if (*count >= max_count) return;
        }
    }
}

int sudoku_solver_count_solutions(int grid[9][9], int max_count) {
    int count = 0;
    int temp[9][9];
    memcpy(temp, grid, sizeof(temp));
    count_solutions_recurse(temp, &count, max_count);
    return count;
}

static void fill_diagonal_boxes(int grid[9][9]) {
    for (int box = 0; box < 9; box += 3) {
        int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        shuffle_array(nums, 9);
        int idx = 0;
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                grid[box + r][box + c] = nums[idx++];
            }
        }
    }
}

void sudoku_generator_generate(SudokuBoard* board, Difficulty diff) {
    if (!board) return;
    
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }
    
    int full_solution[9][9];
    int puzzle[9][9];
    
    memset(full_solution, 0, sizeof(full_solution));
    fill_diagonal_boxes(full_solution);
    solve_backtrack_randomized(full_solution);
    
    memcpy(puzzle, full_solution, sizeof(puzzle));
    
    // Target clues depending on difficulty
    int target_clues = 38;
    switch (diff) {
        case DIFFICULTY_EASY:   target_clues = 40; break;
        case DIFFICULTY_MEDIUM: target_clues = 33; break;
        case DIFFICULTY_HARD:   target_clues = 28; break;
        case DIFFICULTY_EXPERT: target_clues = 24; break;
    }
    
    int positions[81];
    for (int i = 0; i < 81; i++) positions[i] = i;
    shuffle_array(positions, 81);
    
    int current_clues = 81;
    for (int i = 0; i < 81 && current_clues > target_clues; i++) {
        int pos = positions[i];
        int r = pos / 9;
        int c = pos % 9;
        
        int backup = puzzle[r][c];
        puzzle[r][c] = 0;
        
        // Check if puzzle still has a unique solution
        if (sudoku_solver_count_solutions(puzzle, 2) != 1) {
            puzzle[r][c] = backup; // Put back
        } else {
            current_clues--;
        }
    }
    
    // Populate board
    board->difficulty = diff;
    board->mistakes = 0;
    board->hints_used = 0;
    board->elapsed_seconds = 0;
    board->is_paused = false;
    board->is_notes_mode = false;
    board->selected_row = -1;
    board->selected_col = -1;
    board->history_count = 0;
    board->history_index = -1;
    
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            board->solution[r][c] = full_solution[r][c];
            SudokuCell* cell = &board->cells[r][c];
            cell->initial_value = puzzle[r][c];
            cell->value = puzzle[r][c];
            cell->notes = 0;
            cell->is_correct = true;
            cell->is_hinted = false;
        }
    }
    
    // Push initial state to undo stack
    sudoku_push_history(board);
}
