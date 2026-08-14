import { getSudoku } from 'sudoku-gen';
import { Difficulty, SudokuCell } from '../types';

/**
 * Generates a new Sudoku puzzle and solution based on difficulty
 */
export const generateNewPuzzle = (difficulty: Difficulty): { grid: SudokuCell[][], solution: string } => {
  const puzzle = getSudoku(difficulty as any);
  
  const grid: SudokuCell[][] = [];
  const puzzleStr = puzzle.puzzle;
  const solutionStr = puzzle.solution;

  for (let i = 0; i < 9; i++) {
    const row: SudokuCell[] = [];
    for (let j = 0; j < 9; j++) {
      const char = puzzleStr[i * 9 + j];
      const value = char === '-' ? null : parseInt(char, 10);
      row.push({
        value,
        initialValue: value,
        notes: [],
        isCorrect: value !== null ? true : undefined,
        isHinted: false
      });
    }
    grid.push(row);
  }

  return { grid, solution: solutionStr };
};

/**
 * Validates a move against the pre-calculated solution
 */
export const checkMove = (value: number, row: number, col: number, solution: string): boolean => {
  const index = row * 9 + col;
  return parseInt(solution[index], 10) === value;
};

/**
 * Checks whether the entire board matches the solution string
 */
export const isGridComplete = (grid: SudokuCell[][], solution: string): boolean => {
  for (let i = 0; i < 9; i++) {
    for (let j = 0; j < 9; j++) {
      const cell = grid[i][j];
      const solutionVal = parseInt(solution[i * 9 + j], 10);
      if (cell.value !== solutionVal) {
        return false;
      }
    }
  }
  return true;
};

/**
 * Counts how many times each digit (1-9) is still required on the board
 */
export const getRemainingCounts = (grid: SudokuCell[][]): Record<number, number> => {
  const counts: Record<number, number> = {
    1: 9, 2: 9, 3: 9, 4: 9, 5: 9, 6: 9, 7: 9, 8: 9, 9: 9
  };

  for (let i = 0; i < 9; i++) {
    for (let j = 0; j < 9; j++) {
      const val = grid[i][j].value;
      if (val !== null && val >= 1 && val <= 9 && grid[i][j].isCorrect !== false) {
        counts[val] = Math.max(0, counts[val] - 1);
      }
    }
  }

  return counts;
};

/**
 * Checks if placing a value in a cell creates a conflict with its row, column, or 3x3 block
 */
export const hasConflict = (grid: SudokuCell[][], row: number, col: number, value: number | null): boolean => {
  if (value === null) return false;

  // Check row
  for (let c = 0; c < 9; c++) {
    if (c !== col && grid[row][c].value === value) return true;
  }

  // Check column
  for (let r = 0; r < 9; r++) {
    if (r !== row && grid[r][col].value === value) return true;
  }

  // Check 3x3 block
  const startRow = Math.floor(row / 3) * 3;
  const startCol = Math.floor(col / 3) * 3;
  for (let r = startRow; r < startRow + 3; r++) {
    for (let c = startCol; c < startCol + 3; c++) {
      if ((r !== row || c !== col) && grid[r][c].value === value) {
        return true;
      }
    }
  }

  return false;
};

/**
 * Finds a hint for the currently selected cell or the next unsolved cell
 */
export const getHint = (
  grid: SudokuCell[][],
  solution: string,
  selectedCell: [number, number] | null
): { row: number; col: number; value: number } | null => {
  if (selectedCell) {
    const [r, c] = selectedCell;
    const solVal = parseInt(solution[r * 9 + c], 10);
    if (grid[r][c].value !== solVal) {
      return { row: r, col: c, value: solVal };
    }
  }

  for (let i = 0; i < 9; i++) {
    for (let j = 0; j < 9; j++) {
      const solVal = parseInt(solution[i * 9 + j], 10);
      if (grid[i][j].value !== solVal) {
        return { row: i, col: j, value: solVal };
      }
    }
  }

  return null;
};

/**
 * Automatically removes placed digits from candidate notes in row, column, and 3x3 box
 */
export const autoRemoveNotes = (
  grid: SudokuCell[][],
  row: number,
  col: number,
  value: number
): SudokuCell[][] => {
  const startRow = Math.floor(row / 3) * 3;
  const startCol = Math.floor(col / 3) * 3;

  return grid.map((r, ri) =>
    r.map((cell, ci) => {
      if (cell.value !== null || cell.notes.length === 0) return cell;

      const isSameRow = ri === row;
      const isSameCol = ci === col;
      const isSameBox = ri >= startRow && ri < startRow + 3 && ci >= startCol && ci < startCol + 3;

      if (isSameRow || isSameCol || isSameBox) {
        return {
          ...cell,
          notes: cell.notes.filter(n => n !== value)
        };
      }
      return cell;
    })
  );
};
