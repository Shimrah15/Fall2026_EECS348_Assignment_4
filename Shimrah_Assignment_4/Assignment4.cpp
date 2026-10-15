/*
 Program: EECS 348 Assignment 4 - Sudoku Solver
 Description: Read five 9x9 Sudoku puzzles and find every solution using
              an object-oriented recursive depth-first backtracking search.
 Author: Shimrah
 Created: October 14, 2026
 Collaborators: None
 Sources: ChatGPT (https://chatgpt.com/) and Google Gemini
          (https://gemini.google.com/)
*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Based on ChatGPT's original class, with input and search improvements.
class SudokuSolver {
private:
    int grid[9][9]{};    // Zero means the square is empty.
    int solutionCount = 0; // Count every solution found for this puzzle.

    // Check whether a number can be placed at the requested position.
    bool allowed(int row, int col, int value) const {
        // Check the entire row and column for a duplicate number.
        for (int i = 0; i < 9; ++i) {
            if (grid[row][i] == value || grid[i][col] == value)
                return false;
        }
        // Locate the upper-left corner of the matching 3x3 box.
        int boxRow = (row / 3) * 3;
        int boxCol = (col / 3) * 3;
        // Check every square in that box for the same number.
        for (int r = boxRow; r < boxRow + 3; ++r) {
            for (int c = boxCol; c < boxCol + 3; ++c) {
                if (grid[r][c] == value)
                    return false;
            }
        }
        return true; // Number does not conflict with Sudoku rules.
    }

    // Improved search: pick the empty square with the fewest candidates.
    bool findBestEmpty(int& bestRow, int& bestCol) const {
        int fewest = 10; // At most nine numbers could fit a square.
        bool found = false; // Track whether any empty square exists.
        // Examine every square to find the most restricted empty one.
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (grid[row][col] != 0)
                    continue; // Do not change a number from the puzzle.
                found = true;
                int choices = 0; // Count numbers allowed in this square.
                for (int value = 1; value <= 9; ++value) {
                    if (allowed(row, col, value))
                        ++choices;
                }
                // Remember the square with the smallest candidate count.
                if (choices < fewest) {
                       fewest = choices;
                    bestRow = row;
                    bestCol = col;
                }
                if (fewest == 0)
                    return true; // This branch cannot lead to a solution.
            }
        }
        return found; // False means that the entire grid is filled.
    }

    // Print the current Sudoku grid, with underscores for blank squares.
    void printGrid() const {
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                char shown = grid[row][col] == 0 ? '_' : char('0' + grid[row][col]);
                cout << shown << (col == 8 ? '\n' : ' ');
            }
        }
    }

    // Based on ChatGPT's recursive method; uses improved cell selection.
    void solveRecursive() {
        int row = -1, col = -1; // Position selected for the next guess.
        if (!findBestEmpty(row, col)) { // No blanks: a solution is complete.
            ++solutionCount;
            cout << "Solution " << solutionCount << ":\n";
            printGrid(); // Print immediately instead of storing solutions.
            return;
        }
        // Try all possible values for this empty position.
        for (int value = 1; value <= 9; ++value) {
            if (allowed(row, col, value)) {
                grid[row][col] = value; // Make a guess.
                solveRecursive();       // Continue solving this guess.
                grid[row][col] = 0;     // Undo it to explore other guesses.
            }
        }
    }

public:
    // Load exactly 81 squares; reject invalid characters or extra values.
    bool load(const string& fileName) {
        ifstream file(fileName); // Open the named puzzle file.
        if (!file)
            return false; // File could not be opened.
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                char ch;
                if (!(file >> ch) || (ch != '_' && (ch < '1' || ch > '9')))
                    return false; // Missing or illegal character.
                grid[row][col] = ch == '_' ? 0 : ch - '0';
            }
        }
        char extra;
        if (file >> extra)
            return false; // Reject any data after the 81st square.
        // Temporarily remove each clue to detect conflicting clues.
        for (int row = 0; row < 9; ++row) {
            for (int col = 0; col < 9; ++col) {
                if (grid[row][col] == 0)
                    continue;
                int value = grid[row][col];
                grid[row][col] = 0; // Exclude this clue from its own test.
                bool valid = allowed(row, col, value);
                grid[row][col] = value; // Restore the original clue.
                if (!valid)
                    return false; // The clues contradict Sudoku rules.
            }
        }
        solutionCount = 0; // Reset the count for a new file.
        return true; // File contains a well-formed, conflict-free puzzle.
    }

    // Handle one puzzle from file loading through solving and printing.
    void run(const string& fileName) {
        cout << "File: " << fileName << "\n";
        if (!load(fileName)) {
            cout << "Could not read a valid puzzle.\n\n";
            return;
        }
        cout << "Original puzzle:\n";
        printGrid(); // Show the puzzle before filling any squares.
        solveRecursive(); // Search every valid branch recursively.
        if (solutionCount == 0)
            cout << "No solution found\n";
        cout << "\n";
    }
};

// Process the five input files using separate solver objects.
int main() {
    for (int i = 1; i <= 5; ++i) {
        SudokuSolver solver; // Fresh puzzle and solution counter each time.
        solver.run("puzzle" + to_string(i) + ".txt");
    }
    return 0; // All five files have been processed.
}
