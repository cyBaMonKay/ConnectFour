#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>

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

Move miniMax(vector<vector<int>> &boardCopy, bool isMaximizing, int depth, int alpha = -1000, int beta = 1000, int lastRow = -1, int lastCol = -1, vector<int>* heights = nullptr);

// ============ TRANSPOSITION TABLE ============

enum BoundType { EXACT, LOWERBOUND, UPPERBOUND };

struct TTEntry {
    uint64_t hash;
    int depth;
    int score;
    int bestCol;
    BoundType bound;
};

static uint64_t zobristKeys[NUM_ROWS][NUM_COLS][3]; // 0=empty, 1=PLAYER, 2=COMPUTER
static uint64_t zobristTurnKey;
static bool zobristInitialized = false;
static unordered_map<uint64_t, TTEntry> transpositionTable;

static void initZobrist() {
    if (zobristInitialized) return;
    uint64_t s = 0x123456789ABCDEF0ULL;
    auto next = [&]() -> uint64_t {
        s ^= s << 13; s ^= s >> 7; s ^= s << 17; return s;
    };
    for (int r = 0; r < NUM_ROWS; r++)
        for (int c = 0; c < NUM_COLS; c++)
            for (int p = 0; p < 3; p++)
                zobristKeys[r][c][p] = next();
    zobristTurnKey = next();
    zobristInitialized = true;
}

static uint64_t computeBoardHash(const vector<vector<int>>& board, bool isMaximizing) {
    initZobrist();
    uint64_t h = 0;
    for (int r = 0; r < NUM_ROWS; r++)
        for (int c = 0; c < NUM_COLS; c++)
            h ^= zobristKeys[r][c][board[r][c]];
    if (!isMaximizing) h ^= zobristTurnKey;
    return h;
}

void clearTranspositionTable() {
    transpositionTable.clear();
}


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

int getDropRow(vector<vector<int>>& board, int col) {
    for (int r = NUM_ROWS - 1; r >= 0; r--) {
        if (board[r][col] == 0) return r;
    }
    return -1;
}

int getDropRow(vector<int>& heights, int col) {
    return heights[col];
}

void initHeights(vector<vector<int>>& board, vector<int>& heights) {
    for (int c = 0; c < NUM_COLS; c++) {
        heights[c] = getDropRow(board, c);
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

void makeMove(vector<vector<int>>& board, vector<int>& heights, Move move) {
    if (move.player == 0) {  // Undo move: restore height then clear cell
        heights[move.col]++;
        board[heights[move.col]][move.col] = 0;
    } else {  // Make move: place piece then lower height
        board[heights[move.col]][move.col] = move.player;
        heights[move.col]--;
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
    clearTranspositionTable();
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

bool isWinningMove(vector<vector<int>>& board, int player, int row, int col) {
    int count;

    // Horizontal
    count = 1;
    for (int c = col - 1; c >= 0 && board[row][c] == player; c--) count++;
    for (int c = col + 1; c < NUM_COLS && board[row][c] == player; c++) count++;
    if (count >= 4) return true;

    // Vertical
    count = 1;
    for (int r = row - 1; r >= 0 && board[r][col] == player; r--) count++;
    for (int r = row + 1; r < NUM_ROWS && board[r][col] == player; r++) count++;
    if (count >= 4) return true;

    // Diagonal down-right / up-left
    count = 1;
    for (int r = row - 1, c = col - 1; r >= 0 && c >= 0 && board[r][c] == player; r--, c--) count++;
    for (int r = row + 1, c = col + 1; r < NUM_ROWS && c < NUM_COLS && board[r][c] == player; r++, c++) count++;
    if (count >= 4) return true;

    // Diagonal down-left / up-right
    count = 1;
    for (int r = row - 1, c = col + 1; r >= 0 && c < NUM_COLS && board[r][c] == player; r--, c++) count++;
    for (int r = row + 1, c = col - 1; r < NUM_ROWS && c >= 0 && board[r][c] == player; r++, c--) count++;
    if (count >= 4) return true;

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

Move miniMax(vector<vector<int>> &boardCopy, bool isMaximizing, int depth, int alpha, int beta, int lastRow, int lastCol, vector<int>* heights){
    const int moveOrder[NUM_COLS] = {3, 2, 4, 1, 5, 0, 6};

    vector<int> localHeights;
    if (heights == nullptr) {
        localHeights.resize(NUM_COLS);
        initHeights(boardCopy, localHeights);
        heights = &localHeights;
    }

    if (lastRow >= 0) {
        int prevPlayer = isMaximizing ? PLAYER : COMPUTER;
        if (isWinningMove(boardCopy, prevPlayer, lastRow, lastCol)) {
            return Move(-1, 0, (prevPlayer == COMPUTER) ? 1000 : -1000);
        }
    } else {
        if (isWinning(boardCopy, COMPUTER)){
            return Move(-1, 0, 1000);
        }
        if (isWinning(boardCopy, PLAYER)){
            return Move(-1, 0, -1000);
        }
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

    // Transposition table lookup
    uint64_t hash = computeBoardHash(boardCopy, isMaximizing);
    int origAlpha = alpha, origBeta = beta;
    {
        auto it = transpositionTable.find(hash);
        if (it != transpositionTable.end() && it->second.hash == hash && it->second.depth >= depth) {
            const TTEntry& entry = it->second;
            if (entry.bound == EXACT) {
                return Move(entry.bestCol, 0, entry.score);
            } else if (entry.bound == LOWERBOUND) {
                alpha = max(alpha, entry.score);
            } else { // UPPERBOUND
                beta = min(beta, entry.score);
            }
            if (beta <= alpha) {
                return Move(entry.bestCol, 0, entry.score);
            }
        }
    }
    
    if (isMaximizing){
        Move bestMove(-1, COMPUTER, -1000);
        for (int i = 0; i < NUM_COLS; i++){
            int c = moveOrder[i];
            if (boardCopy[0][c] == 0){
                int dropRow = getDropRow(*heights, c);
                makeMove(boardCopy, *heights, Move(c, COMPUTER));
                Move result = miniMax(boardCopy, false, depth - 1, alpha, beta, dropRow, c, heights);
                makeMove(boardCopy, *heights, Move(c, 0)); //undo the move
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
        BoundType bound;
        // Scores are always from the maximizer's perspective.
        // fail-low  (score <= origAlpha): true value is at most this  -> UPPERBOUND
        // fail-high (score >= origBeta) : true value is at least this -> LOWERBOUND
        // otherwise the full window was searched                       -> EXACT
        if (bestMove.score <= origAlpha) bound = UPPERBOUND;
        else if (bestMove.score >= origBeta) bound = LOWERBOUND;
        else bound = EXACT;
        transpositionTable[hash] = {hash, depth, bestMove.score, bestMove.col, bound};
        return bestMove;
    } else {
        Move bestMove(-1, PLAYER, 1000);
        for (int i = 0; i < NUM_COLS; i++){
            int c = moveOrder[i];
            if (boardCopy[0][c] == 0){
                int dropRow = getDropRow(*heights, c);
                makeMove(boardCopy, *heights, Move(c, PLAYER));
                Move result = miniMax(boardCopy, true, depth - 1, alpha, beta, dropRow, c, heights);
                makeMove(boardCopy, *heights, Move(c, 0)); //undo the move
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
        BoundType bound;
        // Same perspective: alpha/beta window is always from the maximizer's view.
        // alpha cutoff (score <= origAlpha): true value <= stored -> UPPERBOUND
        // all moves failed high (score >= origBeta): true value >= stored -> LOWERBOUND
        if (bestMove.score <= origAlpha) bound = UPPERBOUND;
        else if (bestMove.score >= origBeta) bound = LOWERBOUND;
        else bound = EXACT;
        transpositionTable[hash] = {hash, depth, bestMove.score, bestMove.col, bound};
        return bestMove;
    }
}

