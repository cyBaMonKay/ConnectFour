#include <iostream>
#include <vector>

using namespace std;

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))


#define NUM_ROWS 6
#define NUM_COLS 7
#define PLAYER 1
#define COMPUTER 2
#define DEPTH 8


struct Move {
    int col;
    int player;

    Move() : col(-1), player(0) {}
    Move(int c, int p) : col(c), player(p) {}
};


int miniMax(vector<vector<int>> boardCopy, bool isMaximizing, int depth, int alpha = -1000, int beta = 1000);






void printBoard (vector<vector<int>>& board){
    for (int r = 0; r < NUM_ROWS; r++){
        for (int c = 0; c < NUM_COLS; c++){
            cout << "|";
            switch(board[r][c]){
            case 0: cout << " "; break;
            case PLAYER: cout << "X"; break;
            case COMPUTER: cout << "O"; break;
            }
            if ((c + 1) == NUM_COLS){
            cout << "|";
            }
        }
        cout << endl;
    }
}

void initBoard(vector<vector<int>>& board){
    for (int r = 0; r < NUM_ROWS; r++){
        for (int c = 0; c < NUM_COLS; c++){
            board[r][c] = 0;
        }
    }
}

void makeMove(vector<vector<int>>& board, Move move){
    if (move.player == 0){  // Undo move
        for (int r = 0; r < NUM_ROWS; r++){
            if (board[r][move.col] != 0){
                board[r][move.col] = 0;
                break;
            }
        }
    } else {  // Make move
        for (int r = NUM_ROWS - 1; r >= 0; r--){
            if (board[r][move.col] == 0){
                board[r][move.col] = move.player;
                break;
            }
        }
    }
}

Move userMove(vector<vector<int>>& board){
    int col;
    cout << "Enter column (0-" << NUM_COLS - 1 << "): ";
    cin >> col;
    if (col < 0 || col >= NUM_COLS){
        cout << "Invalid column. Try again." << endl;
        return userMove(board);
    }
    if (board[0][col] != 0){  // Check if column is full
        cout << "Column is full. Try again." << endl;
        return userMove(board);
    }
    return Move(col, PLAYER);
}

Move aiMove(vector<vector<int>>& board){
    int bestScore = -1000;
    Move bestMove;
    for (int col = 0; col < NUM_COLS; col++){
        if (board[0][col] == 0){ // Check if column is not full
            makeMove(board, Move(col, COMPUTER));
            int score = miniMax(board, false, DEPTH);
            makeMove(board, Move(col, 0)); // Undo move
            if (score > bestScore){
                bestScore = score;
                bestMove = Move(col, COMPUTER);
            }
        }
    }
    return bestMove;
}

bool isWinning(vector<vector<int>>& board, int player){
    // Check horizontal
    for (int r = 0; r < NUM_ROWS; r++){
        for (int c = 0; c <= NUM_COLS - 4; c++){
            if (board[r][c] == player && 
                board[r][c] == board[r][c+1] && 
                board[r][c] == board[r][c+2] && 
                board[r][c] == board[r][c+3]){
                return true;
            }
        }
    }
    
    // Check vertical
    for (int r = 0; r <= NUM_ROWS - 4; r++){
        for (int c = 0; c < NUM_COLS; c++){
            if (board[r][c] == player && 
                board[r][c] == board[r+1][c] && 
                board[r][c] == board[r+2][c] && 
                board[r][c] == board[r+3][c]){
                return true;
            }
        }
    }
    
    // Check diagonal (top-left to bottom-right)
    for (int r = 0; r <= NUM_ROWS - 4; r++){
        for (int c = 0; c <= NUM_COLS - 4; c++){
            if (board[r][c] == player && 
                board[r][c] == board[r+1][c+1] && 
                board[r][c] == board[r+2][c+2] && 
                board[r][c] == board[r+3][c+3]){
                return true;
            }
        }
    }
    
    // Check diagonal (top-right to bottom-left)
    for (int r = 0; r <= NUM_ROWS - 4; r++){
        for (int c = 3; c < NUM_COLS; c++){
            if (board[r][c] == player && 
                board[r][c] == board[r+1][c-1] && 
                board[r][c] == board[r+2][c-2] && 
                board[r][c] == board[r+3][c-3]){
                return true;
            }
        }
    }
    
    return false;
}

int miniMax(vector<vector<int>> boardCopy, bool isMaximizing, int depth, int alpha, int beta){
    
    if (depth < 1){
        return 0;
    }
    
    if (isWinning(boardCopy, COMPUTER)){
        return 1000;
    } 
    if (isWinning(boardCopy, PLAYER)){
        return -1000;
    }
    
    bool boardFull = true;
    for (int c = 0; c < NUM_COLS; c++){
        if (boardCopy[0][c] == 0){
            boardFull = false;
            break;
        }
    }
    if (boardFull){
        return 0;
    }
    
    if (isMaximizing){
        int maxEval = -1000;
        for (int c = 0; c < NUM_COLS; c++){
            if (boardCopy[0][c] == 0){
                makeMove(boardCopy, Move(c, COMPUTER));
                int eval = miniMax(boardCopy, false, depth - 1, alpha, beta);
                makeMove(boardCopy, Move(c, 0)); //undo the move
                if (eval > maxEval){
                    maxEval = eval;
                }
                if (eval > alpha){
                    alpha = eval;
                }
                if (beta <= alpha){
                    break; // Beta cutoff
                }
            }
        }
        return maxEval;
    } else {
        int minEval = 1000;
        for (int c = 0; c < NUM_COLS; c++){
            if (boardCopy[0][c] == 0){
                makeMove(boardCopy, Move(c, PLAYER));
                int eval = miniMax(boardCopy, true, depth - 1, alpha, beta);
                makeMove(boardCopy, Move(c, 0)); //undo the move
                if (eval < minEval){
                    minEval = eval;
                }
                if (eval < beta){
                    beta = eval;
                }
                if (beta <= alpha){
                    break; // Alpha cutoff
                }
            }
        }
        return minEval;
    }
}
