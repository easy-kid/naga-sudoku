#include "sudoku_core.h"
#include <string.h>

void sudoku_cell_clear(SudokuCell* cell) {
    if (!cell) return;
    cell->value = 0;
    cell->initial_value = 0;
    cell->notes = 0;
    cell->is_correct = true;
    cell->is_hinted = false;
}

bool sudoku_cell_has_note(const SudokuCell* cell, int digit) {
    if (!cell || digit < 1 || digit > 9) return false;
    return (cell->notes & (1 << digit)) != 0;
}

void sudoku_cell_toggle_note(SudokuCell* cell, int digit) {
    if (!cell || digit < 1 || digit > 9) return;
    cell->notes ^= (1 << digit);
}

void sudoku_cell_remove_note(SudokuCell* cell, int digit) {
    if (!cell || digit < 1 || digit > 9) return;
    cell->notes &= ~(1 << digit);
}

void sudoku_push_history(SudokuBoard* board) {
    if (!board) return;
    
    // If we performed an action after some undos, truncate redo history
    if (board->history_index < board->history_count - 1) {
        board->history_count = board->history_index + 1;
    }
    
    // If history is full, shift left
    if (board->history_count >= MAX_HISTORY) {
        memmove(&board->history[0], &board->history[1], sizeof(HistoryStep) * (MAX_HISTORY - 1));
        board->history_count = MAX_HISTORY - 1;
        board->history_index = MAX_HISTORY - 2;
    }
    
    HistoryStep* step = &board->history[board->history_count];
    memcpy(step->cells, board->cells, sizeof(board->cells));
    step->selected_row = board->selected_row;
    step->selected_col = board->selected_col;
    
    board->history_index = board->history_count;
    board->history_count++;
}

bool sudoku_undo(SudokuBoard* board) {
    if (!board || board->history_index <= 0) return false;
    
    board->history_index--;
    HistoryStep* step = &board->history[board->history_index];
    memcpy(board->cells, step->cells, sizeof(board->cells));
    board->selected_row = step->selected_row;
    board->selected_col = step->selected_col;
    return true;
}

bool sudoku_redo(SudokuBoard* board) {
    if (!board || board->history_index >= board->history_count - 1) return false;
    
    board->history_index++;
    HistoryStep* step = &board->history[board->history_index];
    memcpy(board->cells, step->cells, sizeof(board->cells));
    board->selected_row = step->selected_row;
    board->selected_col = step->selected_col;
    return true;
}

void sudoku_auto_remove_peer_notes(SudokuBoard* board, int row, int col, int val) {
    if (!board || val < 1 || val > 9) return;
    
    int start_r = (row / 3) * 3;
    int start_c = (col / 3) * 3;
    
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (board->cells[r][c].value == 0 && board->cells[r][c].notes != 0) {
                bool is_same_row = (r == row);
                bool is_same_col = (c == col);
                bool is_same_box = (r >= start_r && r < start_r + 3 && c >= start_c && c < start_c + 3);
                
                if (is_same_row || is_same_col || is_same_box) {
                    sudoku_cell_remove_note(&board->cells[r][c], val);
                }
            }
        }
    }
}

bool sudoku_set_number(SudokuBoard* board, int row, int col, int num, bool strict_mistakes, bool auto_remove_notes) {
    if (!board || row < 0 || row >= 9 || col < 0 || col >= 9) return false;
    SudokuCell* cell = &board->cells[row][col];
    
    // Cannot modify initial puzzle givens
    if (cell->initial_value != 0) return false;
    
    if (num < 1 || num > 9) {
        cell->value = 0;
        cell->is_correct = true;
        cell->is_hinted = false;
        sudoku_push_history(board);
        return true;
    }
    
    int sol = board->solution[row][col];
    cell->value = num;
    cell->notes = 0; // Clear notes in this cell when digit is placed
    
    bool is_correct = (num == sol);
    if (is_correct) {
        cell->is_correct = true;
        if (auto_remove_notes) {
            sudoku_auto_remove_peer_notes(board, row, col, num);
        }
    } else {
        cell->is_correct = false;
        if (strict_mistakes) {
            board->mistakes++;
        }
    }
    
    sudoku_push_history(board);
    return is_correct;
}

void sudoku_toggle_cell_note(SudokuBoard* board, int row, int col, int num) {
    if (!board || row < 0 || row >= 9 || col < 0 || col >= 9) return;
    SudokuCell* cell = &board->cells[row][col];
    
    if (cell->value != 0 || cell->initial_value != 0) return;
    if (num < 1 || num > 9) return;
    
    sudoku_cell_toggle_note(cell, num);
    sudoku_push_history(board);
}

void sudoku_erase_cell(SudokuBoard* board, int row, int col) {
    if (!board || row < 0 || row >= 9 || col < 0 || col >= 9) return;
    SudokuCell* cell = &board->cells[row][col];
    
    if (cell->initial_value != 0) return;
    if (cell->value == 0 && cell->notes == 0) return;
    
    cell->value = 0;
    cell->notes = 0;
    cell->is_correct = true;
    cell->is_hinted = false;
    sudoku_push_history(board);
}

bool sudoku_request_hint(SudokuBoard* board, int* out_row, int* out_col, int* out_val) {
    if (!board) return false;
    
    int target_r = -1;
    int target_c = -1;
    
    // 1. If user has a selected cell that is not yet correct, solve it
    if (board->selected_row >= 0 && board->selected_row < 9 &&
        board->selected_col >= 0 && board->selected_col < 9) {
        int sr = board->selected_row;
        int sc = board->selected_col;
        if (board->cells[sr][sc].value != board->solution[sr][sc]) {
            target_r = sr;
            target_c = sc;
        }
    }
    
    // 2. Otherwise find the first unsolved cell
    if (target_r == -1) {
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                if (board->cells[r][c].value != board->solution[r][c]) {
                    target_r = r;
                    target_c = c;
                    break;
                }
            }
            if (target_r != -1) break;
        }
    }
    
    if (target_r == -1) return false; // Puzzle already complete
    
    int sol = board->solution[target_r][target_c];
    SudokuCell* cell = &board->cells[target_r][target_c];
    cell->value = sol;
    cell->notes = 0;
    cell->is_correct = true;
    cell->is_hinted = true;
    
    board->selected_row = target_r;
    board->selected_col = target_c;
    board->hints_used++;
    
    sudoku_auto_remove_peer_notes(board, target_r, target_c, sol);
    sudoku_push_history(board);
    
    if (out_row) *out_row = target_r;
    if (out_col) *out_col = target_c;
    if (out_val) *out_val = sol;
    
    return true;
}

bool sudoku_is_complete(const SudokuBoard* board) {
    if (!board) return false;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (board->cells[r][c].value != board->solution[r][c]) {
                return false;
            }
        }
    }
    return true;
}

void sudoku_get_remaining_counts(const SudokuBoard* board, int counts[10]) {
    if (!board || !counts) return;
    for (int i = 1; i <= 9; i++) {
        counts[i] = 9;
    }
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int val = board->cells[r][c].value;
            if (val >= 1 && val <= 9 && board->cells[r][c].is_correct) {
                if (counts[val] > 0) counts[val]--;
            }
        }
    }
}

bool sudoku_has_conflict(const SudokuBoard* board, int row, int col, int val) {
    if (!board || val < 1 || val > 9) return false;
    
    // Check row
    for (int c = 0; c < 9; c++) {
        if (c != col && board->cells[row][c].value == val) return true;
    }
    
    // Check col
    for (int r = 0; r < 9; r++) {
        if (r != row && board->cells[r][col].value == val) return true;
    }
    
    // Check box
    int start_r = (row / 3) * 3;
    int start_c = (col / 3) * 3;
    for (int r = start_r; r < start_r + 3; r++) {
        for (int c = start_c; c < start_c + 3; c++) {
            if ((r != row || c != col) && board->cells[r][c].value == val) return true;
        }
    }
    
    return false;
}
