#include <stdio.h>
#include <assert.h>
#include "sudoku_core.h"
#include "sudoku_generator.h"

int main() {
    printf("=== Testing Sudoku Core & Generator in C ===\n");
    
    SudokuBoard board;
    
    // 1. Generate Easy Board
    sudoku_generator_generate(&board, DIFFICULTY_EASY);
    printf("[PASS] Generated Easy Board with unique solution.\n");
    
    // Check initial clues count
    int clues = 0;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (board.cells[r][c].initial_value != 0) clues++;
        }
    }
    printf("Clues count: %d\n", clues);
    assert(clues >= 35 && clues <= 45);
    
    // 2. Test solver uniqueness on the puzzle
    int puzzle_copy[9][9];
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            puzzle_copy[r][c] = board.cells[r][c].initial_value;
        }
    }
    int sol_count = sudoku_solver_count_solutions(puzzle_copy, 2);
    assert(sol_count == 1);
    printf("[PASS] Puzzle verified to have EXACTLY 1 unique solution.\n");
    
    // 3. Test note toggling
    SudokuCell test_cell;
    sudoku_cell_clear(&test_cell);
    assert(test_cell.notes == 0);
    sudoku_cell_toggle_note(&test_cell, 4);
    assert(sudoku_cell_has_note(&test_cell, 4));
    assert(!sudoku_cell_has_note(&test_cell, 5));
    sudoku_cell_toggle_note(&test_cell, 4);
    assert(!sudoku_cell_has_note(&test_cell, 4));
    printf("[PASS] Bitmask notes system verified.\n");
    
    // 4. Test Undo & Redo
    int empty_r = -1, empty_c = -1;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (board.cells[r][c].initial_value == 0) {
                empty_r = r;
                empty_c = c;
                break;
            }
        }
        if (empty_r != -1) break;
    }
    
    int correct_val = board.solution[empty_r][empty_c];
    int wrong_val = (correct_val % 9) + 1;
    
    // Put wrong number with strict mistakes
    bool res = sudoku_set_number(&board, empty_r, empty_c, wrong_val, true, true);
    assert(!res);
    assert(board.cells[empty_r][empty_c].value == wrong_val);
    assert(!board.cells[empty_r][empty_c].is_correct);
    assert(board.mistakes == 1);
    
    // Undo
    assert(sudoku_undo(&board));
    assert(board.cells[empty_r][empty_c].value == 0);
    
    // Redo
    assert(sudoku_redo(&board));
    assert(board.cells[empty_r][empty_c].value == wrong_val);
    
    // Erase
    sudoku_erase_cell(&board, empty_r, empty_c);
    assert(board.cells[empty_r][empty_c].value == 0);
    printf("[PASS] Move validation, mistakes and Undo/Redo stack verified.\n");
    
    // 5. Test hint
    int h_row, h_col, h_val;
    assert(sudoku_request_hint(&board, &h_row, &h_col, &h_val));
    assert(board.cells[h_row][h_col].value == board.solution[h_row][h_col]);
    assert(board.cells[h_row][h_col].is_hinted);
    assert(board.hints_used == 1);
    printf("[PASS] Hint logic verified.\n");
    
    printf("\n>>> ALL SUDOKU CORE TESTS PASSED SUCCESSFULLY! <<<\n");
    return 0;
}
