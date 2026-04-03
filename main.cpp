#include <iostream>
#include <vector>
#include "connectfour.h"

using namespace std;

// Clears the terminal screen using ANSI escape codes.
// \033[2J clears the screen; \033[H moves the cursor to the home position.
// Works on ANSI-compatible terminals (Unix/Linux/macOS and modern Windows).
void clearConsole() {
    cout << "\033[2J\033[H" << flush;
}

int main (int argc, char** argv) {
    vector<vector<int>> board (NUM_ROWS, vector<int> (NUM_COLS));
    bool gameOver = false;
    unsigned int turns = 0;
    initBoard(board);

    while (!gameOver){
        if (turns % 2 == 0){ // Player's turn
            clearConsole();
            printBoard(board);
            cout << "================================" << endl;
            makeMove(board, userMove(board));
        }  
         else { // Computer's turn
            makeMove(board, aiMove(board));
            clearConsole();
            printBoard(board);
            cout << "================================" << endl;
        }

        turns++;
        gameOver = isWinning(board, PLAYER) || isWinning(board, COMPUTER) || turns == NUM_ROWS * NUM_COLS;
        
    }

    cout << "Game Over!" << endl;
    if (isWinning(board, PLAYER)){
        cout << "Congratulations! You win!" << endl;
    } else if (isWinning(board, COMPUTER)){
        cout << "Computer wins! Better luck next time." << endl;
    } else {
        cout << "It's a draw!" << endl;
    }

    return 0;
}