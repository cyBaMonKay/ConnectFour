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
    int score;

    Move() : col(-1), player(0), score(0) {}
    Move(int c, int p) : col(c), player(p), score(0) {}
    Move(int c, int p, int s) : col(c), player(p), score(s) {}
};

Move miniMax(vector<vector<int>> boardCopy, bool isMaximizing, int depth, int alpha = -1000, int beta = 1000);





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
    Move best = miniMax(board, true, DEPTH);
    best.player = COMPUTER;
    return best;
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

int evaluateWindow(vector<int>& window) {
    int score = 0;
    int computerCount = 0, playerCount = 0, emptyCount = 0;
    for (int cell : window) {
        if      (cell == COMPUTER) computerCount++;
        else if (cell == PLAYER)   playerCount++;
        else                       emptyCount++;
    }
    if      (computerCount == 3 && emptyCount == 1) score += 5;
    else if (computerCount == 2 && emptyCount == 2) score += 2;

    if      (playerCount == 3 && emptyCount == 1) score -= 5;
    else if (playerCount == 2 && emptyCount == 2) score -= 2;

    return score;
}

int evaluateBoard(vector<vector<int>>& board) {
    if (isWinning(board, COMPUTER)) return 1000;
    if (isWinning(board, PLAYER))   return -1000;

    int score = 0;

    // Center column preference
    int centerCol = NUM_COLS / 2;
    for (int r = 0; r < NUM_ROWS; r++) {
        if      (board[r][centerCol] == COMPUTER) score += 3;
        else if (board[r][centerCol] == PLAYER)   score -= 3;
    }

    // Horizontal windows
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c <= NUM_COLS - 4; c++) {
            vector<int> window = {board[r][c], board[r][c+1], board[r][c+2], board[r][c+3]};
            score += evaluateWindow(window);
        }
    }

    // Vertical windows
    for (int r = 0; r <= NUM_ROWS - 4; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            vector<int> window = {board[r][c], board[r+1][c], board[r+2][c], board[r+3][c]};
            score += evaluateWindow(window);
        }
    }

    // Diagonal (top-left to bottom-right)
    for (int r = 0; r <= NUM_ROWS - 4; r++) {
        for (int c = 0; c <= NUM_COLS - 4; c++) {
            vector<int> window = {board[r][c], board[r+1][c+1], board[r+2][c+2], board[r+3][c+3]};
            score += evaluateWindow(window);
        }
    }

    // Diagonal (top-right to bottom-left)
    for (int r = 0; r <= NUM_ROWS - 4; r++) {
        for (int c = 3; c < NUM_COLS; c++) {
            vector<int> window = {board[r][c], board[r+1][c-1], board[r+2][c-2], board[r+3][c-3]};
            score += evaluateWindow(window);
        }
    }

    return score;
}

Move miniMax(vector<vector<int>> boardCopy, bool isMaximizing, int depth, int alpha, int beta){
    if (isWinning(boardCopy, COMPUTER)){
        return Move(-1, 0, 1000);
    }
    if (isWinning(boardCopy, PLAYER)){
        return Move(-1, 0, -1000);
    }

    if (depth < 1){
        return Move(-1, 0, evaluateBoard(boardCopy));
    }
    
    bool boardFull = true;
    for (int c = 0; c < NUM_COLS; c++){
        if (boardCopy[0][c] == 0){
            boardFull = false;
            break;
        }
    }
    if (boardFull){
        return Move(-1, 0, 0);
    }
    
    if (isMaximizing){
        Move bestMove(-1, COMPUTER, -1000);
        for (int c = 0; c < NUM_COLS; c++){
            if (boardCopy[0][c] == 0){
                makeMove(boardCopy, Move(c, COMPUTER));
                Move result = miniMax(boardCopy, false, depth - 1, alpha, beta);
                makeMove(boardCopy, Move(c, 0)); //undo the move
                if (result.score > bestMove.score){
                    bestMove = Move(c, COMPUTER, result.score);
                }
                if (result.score > alpha){
                    alpha = result.score;
                }
                if (beta <= alpha){
                    break; // Beta cutoff
                }
            }
        }
        return bestMove;
    } else {
        Move bestMove(-1, PLAYER, 1000);
        for (int c = 0; c < NUM_COLS; c++){
            if (boardCopy[0][c] == 0){
                makeMove(boardCopy, Move(c, PLAYER));
                Move result = miniMax(boardCopy, true, depth - 1, alpha, beta);
                makeMove(boardCopy, Move(c, 0)); //undo the move
                if (result.score < bestMove.score){
                    bestMove = Move(c, PLAYER, result.score);
                }
                if (result.score < beta){
                    beta = result.score;
                }
                if (beta <= alpha){
                    break; // Alpha cutoff
                }
            }
        }
        return bestMove;
    }
}



