#include <iostream>
#include <vector>
#include "connectfour.h"

using namespace std;

int main (int argc, char** argv) {
    vector<vector<int>> board (NUM_ROWS, vector<int> (NUM_COLS));
    bool gameOver = false;
    unsigned int turns = 0;
    initBoard(board);

    while (!gameOver){
        if (turns % 2 == 0){ // Player's turn
            makeMove(board, userMove(board));
        }  
         else { // Computer's turn
            makeMove(board, aiMove(board));
            printBoard(board);
            cout << "================================" << endl;
        }

        gameOver = isWinning(board, PLAYER) || isWinning(board, COMPUTER) || turns == NUM_ROWS * NUM_COLS;
        turns++;
        
    }

    return 0;
}