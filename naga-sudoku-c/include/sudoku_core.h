#ifndef SUDOKU_CORE_H
#define SUDOKU_CORE_H

#include <stdbool.h>
#include <stdint.h>

#define GRID_SIZE 9
#define SUBGRID_SIZE 3
#define MAX_MISTAKES 3
#define MAX_HISTORY 64

typedef enum {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_MEDIUM = 1,
    DIFFICULTY_HARD = 2,
    DIFFICULTY_EXPERT = 3
} Difficulty;

typedef struct {
    int value;           // 0 = empty, 1-9 = filled
    int initial_value;   // 0 = empty, 1-9 = given in puzzle
    uint16_t notes;      // Bitmask: bit 1 -> digit 1, bit 2 -> digit 2, ..., bit 9 -> digit 9
    bool is_correct;     // true if matches solution or initial
    bool is_hinted;      // true if solved via hint
} SudokuCell;

typedef struct {
    SudokuCell cells[GRID_SIZE][GRID_SIZE];
    int selected_row;
    int selected_col;
} HistoryStep;

typedef struct {
    SudokuCell cells[GRID_SIZE][GRID_SIZE];
    int solution[GRID_SIZE][GRID_SIZE];
    Difficulty difficulty;
    
    int mistakes;
    int hints_used;
    int elapsed_seconds;
    bool is_paused;
    bool is_notes_mode;
    
    int selected_row;
    int selected_col;
    
    // History stack for undo / redo
    HistoryStep history[MAX_HISTORY];
    int history_count;
    int history_index; // -1 if empty, index in history array
} SudokuBoard;

// Core functions
void sudoku_cell_clear(SudokuCell* cell);
bool sudoku_cell_has_note(const SudokuCell* cell, int digit);
void sudoku_cell_toggle_note(SudokuCell* cell, int digit);
void sudoku_cell_remove_note(SudokuCell* cell, int digit);

// Board initialization & history
void sudoku_board_init(SudokuBoard* board, Difficulty diff);
void sudoku_push_history(SudokuBoard* board);
bool sudoku_undo(SudokuBoard* board);
bool sudoku_redo(SudokuBoard* board);

// Player actions
bool sudoku_set_number(SudokuBoard* board, int row, int col, int num, bool strict_mistakes, bool auto_remove_notes);
void sudoku_toggle_cell_note(SudokuBoard* board, int row, int col, int num);
void sudoku_erase_cell(SudokuBoard* board, int row, int col);
bool sudoku_request_hint(SudokuBoard* board, int* out_row, int* out_col, int* out_val);

// State queries
bool sudoku_is_complete(const SudokuBoard* board);
void sudoku_get_remaining_counts(const SudokuBoard* board, int counts[10]);
bool sudoku_has_conflict(const SudokuBoard* board, int row, int col, int val);
void sudoku_auto_remove_peer_notes(SudokuBoard* board, int row, int col, int val);

#endif // SUDOKU_CORE_H
