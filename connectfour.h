#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <array>
#include <algorithm>

using namespace std;

#define NUM_ROWS 6
#define NUM_COLS 7
#define PLAYER 1
#define COMPUTER 2
#define DEPTH 16

struct Move {
    int col;
    int player;
    int score;

    Move() : col(-1), player(0), score(0) {}
    Move(int c, int p) : col(c), player(p), score(0) {}
    Move(int c, int p, int s) : col(c), player(p), score(s) {}
};

struct BitBoardState {
    uint64_t playerBits;
    uint64_t computerBits;
    uint64_t occupancy;
    array<int, NUM_COLS> heights; // Number of discs in each column [0..6]

    BitBoardState() : playerBits(0), computerBits(0), occupancy(0) {
        heights.fill(0);
    }
};

inline int bitIndexFromGrid(int row, int col) {
    // Board rows are top-based [0..5], bit rows are bottom-based [0..5]
    const int bitRow = NUM_ROWS - 1 - row;
    return col * 7 + bitRow;
}

inline uint64_t slotMaskFromGrid(int row, int col) {
    return 1ULL << bitIndexFromGrid(row, col);
}

inline uint64_t slotMaskFromBitRow(int bitRow, int col) {
    return 1ULL << (col * 7 + bitRow);
}

inline bool hasConnect4(uint64_t bits) {
    uint64_t m = bits & (bits >> 1);   // vertical
    if (m & (m >> 2)) return true;

    m = bits & (bits >> 7);            // horizontal
    if (m & (m >> 14)) return true;

    m = bits & (bits >> 6);            // diagonal (/)
    if (m & (m >> 12)) return true;

    m = bits & (bits >> 8);            // diagonal (\)
    if (m & (m >> 16)) return true;

    return false;
}

inline int getCell(const BitBoardState& state, int row, int col) {
    const uint64_t mask = slotMaskFromGrid(row, col);
    if (state.playerBits & mask) return PLAYER;
    if (state.computerBits & mask) return COMPUTER;
    return 0;
}

inline void initBoard(BitBoardState& state) {
    state.playerBits = 0;
    state.computerBits = 0;
    state.occupancy = 0;
    state.heights.fill(0);
}

inline BitBoardState boardToBitBoard(const vector<vector<int>>& board) {
    BitBoardState state;

    for (int c = 0; c < NUM_COLS; c++) {
        int maxOccupiedBitRow = -1;

        for (int r = 0; r < NUM_ROWS; r++) {
            const int cell = board[r][c];
            if (cell == 0) continue;

            const int bitRow = NUM_ROWS - 1 - r;
            const uint64_t mask = slotMaskFromBitRow(bitRow, c);
            if (cell == PLAYER) {
                state.playerBits |= mask;
            } else if (cell == COMPUTER) {
                state.computerBits |= mask;
            }
            if (bitRow > maxOccupiedBitRow) {
                maxOccupiedBitRow = bitRow;
            }
        }

        state.heights[c] = maxOccupiedBitRow + 1;
    }

    state.occupancy = state.playerBits | state.computerBits;
    return state;
}

inline void bitBoardToBoard(const BitBoardState& state, vector<vector<int>>& board) {
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            board[r][c] = getCell(state, r, c);
        }
    }
}

inline bool isLegalMove(const BitBoardState& state, int col) {
    return col >= 0 && col < NUM_COLS && state.heights[col] < NUM_ROWS;
}

inline int getDropRow(const BitBoardState& state, int col) {
    if (!isLegalMove(state, col)) return -1;
    const int bitRow = state.heights[col];
    return NUM_ROWS - 1 - bitRow;
}

inline void makeMove(BitBoardState& state, Move move) {
    if (move.col < 0 || move.col >= NUM_COLS) return;

    if (move.player == 0) {
        // Undo top-most piece in the column.
        if (state.heights[move.col] <= 0) return;
        const int bitRow = state.heights[move.col] - 1;
        const uint64_t mask = slotMaskFromBitRow(bitRow, move.col);
        state.playerBits &= ~mask;
        state.computerBits &= ~mask;
        state.occupancy &= ~mask;
        state.heights[move.col]--;
        return;
    }

    if (!isLegalMove(state, move.col)) return;

    const int bitRow = state.heights[move.col];
    const uint64_t mask = slotMaskFromBitRow(bitRow, move.col);
    if (move.player == PLAYER) {
        state.playerBits |= mask;
    } else if (move.player == COMPUTER) {
        state.computerBits |= mask;
    }
    state.occupancy |= mask;
    state.heights[move.col]++;
}

inline void printBoard(BitBoardState& state) {
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            cout << "|";
            switch (getCell(state, r, c)) {
                case 0: cout << " "; break;
                case PLAYER: cout << "X"; break;
                case COMPUTER: cout << "O"; break;
            }
            if (c + 1 == NUM_COLS) {
                cout << "|";
            }
        }
        cout << endl;
    }
}

inline Move userMove(BitBoardState& state) {
    int col;
    cout << "Enter column (0-" << NUM_COLS - 1 << "): ";
    cin >> col;
    if (col < 0 || col >= NUM_COLS) {
        cout << "Invalid column. Try again." << endl;
        return userMove(state);
    }
    if (!isLegalMove(state, col)) {
        cout << "Column is full. Try again." << endl;
        return userMove(state);
    }
    return Move(col, PLAYER);
}

inline bool isWinning(const BitBoardState& state, int player) {
    if (player == PLAYER) return hasConnect4(state.playerBits);
    if (player == COMPUTER) return hasConnect4(state.computerBits);
    return false;
}

inline bool isWinningMove(const BitBoardState& state, int player, int row, int col) {
    (void)row;
    (void)col;
    return isWinning(state, player);
}

inline int evaluateWindow(vector<int>& window) {
    int score = 0;
    int computerCount = 0;
    int playerCount = 0;
    int emptyCount = 0;

    for (int cell : window) {
        if (cell == COMPUTER) computerCount++;
        else if (cell == PLAYER) playerCount++;
        else emptyCount++;
    }

    if (computerCount == 3 && emptyCount == 1) score += 5;
    else if (computerCount == 2 && emptyCount == 2) score += 2;

    if (playerCount == 3 && emptyCount == 1) score -= 5;
    else if (playerCount == 2 && emptyCount == 2) score -= 2;

    return score;
}

inline int evaluateWindowCells(int cell0, int cell1, int cell2, int cell3) {
    int score = 0;
    int computerCount = 0;
    int playerCount = 0;
    int emptyCount = 0;
    const int cells[4] = {cell0, cell1, cell2, cell3};

    for (int cell : cells) {
        if (cell == COMPUTER) computerCount++;
        else if (cell == PLAYER) playerCount++;
        else emptyCount++;
    }

    if (computerCount == 3 && emptyCount == 1) score += 5;
    else if (computerCount == 2 && emptyCount == 2) score += 2;

    if (playerCount == 3 && emptyCount == 1) score -= 5;
    else if (playerCount == 2 && emptyCount == 2) score -= 2;

    return score;
}

inline int evaluateBoard(const BitBoardState& state) {
    if (isWinning(state, COMPUTER)) return 1000;
    if (isWinning(state, PLAYER)) return -1000;

    int score = 0;

    const int centerCol = NUM_COLS / 2;
    for (int r = 0; r < NUM_ROWS; r++) {
        const int cell = getCell(state, r, centerCol);
        if (cell == COMPUTER) score += 3;
        else if (cell == PLAYER) score -= 3;
    }

    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c <= NUM_COLS - 4; c++) {
            score += evaluateWindowCells(
                getCell(state, r, c),
                getCell(state, r, c + 1),
                getCell(state, r, c + 2),
                getCell(state, r, c + 3)
            );
        }
    }

    for (int r = 0; r <= NUM_ROWS - 4; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            score += evaluateWindowCells(
                getCell(state, r, c),
                getCell(state, r + 1, c),
                getCell(state, r + 2, c),
                getCell(state, r + 3, c)
            );
        }
    }

    for (int r = 0; r <= NUM_ROWS - 4; r++) {
        for (int c = 0; c <= NUM_COLS - 4; c++) {
            score += evaluateWindowCells(
                getCell(state, r, c),
                getCell(state, r + 1, c + 1),
                getCell(state, r + 2, c + 2),
                getCell(state, r + 3, c + 3)
            );
        }
    }

    for (int r = 0; r <= NUM_ROWS - 4; r++) {
        for (int c = 3; c < NUM_COLS; c++) {
            score += evaluateWindowCells(
                getCell(state, r, c),
                getCell(state, r + 1, c - 1),
                getCell(state, r + 2, c - 2),
                getCell(state, r + 3, c - 3)
            );
        }
    }

    return score;
}

enum BoundType { EXACT, LOWERBOUND, UPPERBOUND };

struct TTEntry {
    uint64_t hash;
    int depth;
    int score;
    int bestCol;
    BoundType bound;
};

static unordered_map<uint64_t, TTEntry> transpositionTable;

inline uint64_t computeBoardHash(const BitBoardState& state, bool isMaximizing) {
    // Lightweight mix for transposition keys.
    uint64_t h = 1469598103934665603ULL;
    h ^= state.playerBits + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    h ^= state.computerBits + 0xc2b2ae3d27d4eb4fULL + (h << 6) + (h >> 2);
    if (!isMaximizing) {
        h ^= 0x517cc1b727220a95ULL;
    }
    return h;
}

inline void clearTranspositionTable() {
    transpositionTable.clear();
}

Move miniMax(BitBoardState& state, bool isMaximizing, int depth, int alpha = -1000, int beta = 1000, int lastCol = -1);
Move tacticalPrepass(BitBoardState& state);
Move aiMove(BitBoardState& state);

inline Move tacticalPrepass(BitBoardState& state) {
    for (int c = 0; c < NUM_COLS; c++) {
        if (!isLegalMove(state, c)) continue;
        makeMove(state, Move(c, COMPUTER));
        const bool wins = isWinning(state, COMPUTER);
        makeMove(state, Move(c, 0));
        if (wins) return Move(c, COMPUTER, 1000);
    }

    for (int c = 0; c < NUM_COLS; c++) {
        if (!isLegalMove(state, c)) continue;
        makeMove(state, Move(c, PLAYER));
        const bool playerWins = isWinning(state, PLAYER);
        makeMove(state, Move(c, 0));
        if (playerWins) return Move(c, COMPUTER, 0);
    }

    return Move(-1, COMPUTER, 0);
}

inline Move miniMax(BitBoardState& state, bool isMaximizing, int depth, int alpha, int beta, int lastCol) {
    const int moveOrder[NUM_COLS] = {3, 2, 4, 1, 5, 0, 6};

    if (lastCol >= 0) {
        const int prevPlayer = isMaximizing ? PLAYER : COMPUTER;
        if (isWinning(state, prevPlayer)) {
            return Move(-1, 0, (prevPlayer == COMPUTER) ? 1000 : -1000);
        }
    } else {
        if (isWinning(state, COMPUTER)) return Move(-1, 0, 1000);
        if (isWinning(state, PLAYER)) return Move(-1, 0, -1000);
    }

    if (depth < 1) {
        return Move(-1, 0, evaluateBoard(state));
    }

    bool boardFull = true;
    for (int c = 0; c < NUM_COLS; c++) {
        if (isLegalMove(state, c)) {
            boardFull = false;
            break;
        }
    }
    if (boardFull) {
        return Move(-1, 0, 0);
    }

    const uint64_t hash = computeBoardHash(state, isMaximizing);
    const int origAlpha = alpha;
    const int origBeta = beta;
    int ttBestCol = -1;

    auto it = transpositionTable.find(hash);
    if (it != transpositionTable.end() && it->second.hash == hash) {
        ttBestCol = it->second.bestCol;
        if (it->second.depth >= depth) {
            const TTEntry& entry = it->second;
            if (entry.bound == EXACT) {
                return Move(entry.bestCol, 0, entry.score);
            }
            if (entry.bound == LOWERBOUND) {
                alpha = std::max(alpha, entry.score);
            } else {
                beta = std::min(beta, entry.score);
            }
            if (beta <= alpha) {
                return Move(entry.bestCol, 0, entry.score);
            }
        }
    }

    int orderedMoves[NUM_COLS];
    int numMoves = 0;

    if (ttBestCol >= 0 && ttBestCol < NUM_COLS && isLegalMove(state, ttBestCol)) {
        orderedMoves[numMoves++] = ttBestCol;
    }
    for (int i = 0; i < NUM_COLS; i++) {
        const int c = moveOrder[i];
        if (c != ttBestCol && isLegalMove(state, c)) {
            orderedMoves[numMoves++] = c;
        }
    }

    if (isMaximizing) {
        Move bestMove(-1, COMPUTER, -1000);
        for (int i = 0; i < numMoves; i++) {
            const int c = orderedMoves[i];
            makeMove(state, Move(c, COMPUTER));
            const Move result = miniMax(state, false, depth - 1, alpha, beta, c);
            makeMove(state, Move(c, 0));

            if (result.score > bestMove.score) {
                bestMove = Move(c, COMPUTER, result.score);
            }
            if (result.score > alpha) {
                alpha = result.score;
            }
            if (beta <= alpha) {
                break;
            }
        }

        BoundType bound = EXACT;
        if (bestMove.score <= origAlpha) bound = UPPERBOUND;
        else if (bestMove.score >= origBeta) bound = LOWERBOUND;

        transpositionTable[hash] = {hash, depth, bestMove.score, bestMove.col, bound};
        return bestMove;
    }

    Move bestMove(-1, PLAYER, 1000);
    for (int i = 0; i < numMoves; i++) {
        const int c = orderedMoves[i];
        makeMove(state, Move(c, PLAYER));
        const Move result = miniMax(state, true, depth - 1, alpha, beta, c);
        makeMove(state, Move(c, 0));

        if (result.score < bestMove.score) {
            bestMove = Move(c, PLAYER, result.score);
        }
        if (result.score < beta) {
            beta = result.score;
        }
        if (beta <= alpha) {
            break;
        }
    }

    BoundType bound = EXACT;
    if (bestMove.score <= origAlpha) bound = UPPERBOUND;
    else if (bestMove.score >= origBeta) bound = LOWERBOUND;

    transpositionTable[hash] = {hash, depth, bestMove.score, bestMove.col, bound};
    return bestMove;
}

inline Move aiMove(BitBoardState& state) {
    Move tactic = tacticalPrepass(state);
    if (tactic.col >= 0) {
        return tactic;
    }

    clearTranspositionTable();
    Move best(-1, COMPUTER, -1000);
    for (int d = 1; d <= DEPTH; d++) {
        best = miniMax(state, true, d);
    }
    best.player = COMPUTER;
    return best;
}

// ===== Compatibility vector-based API (used by tests and legacy callers) =====

inline void printBoard(vector<vector<int>>& board) {
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            cout << "|";
            switch (board[r][c]) {
                case 0: cout << " "; break;
                case PLAYER: cout << "X"; break;
                case COMPUTER: cout << "O"; break;
            }
            if ((c + 1) == NUM_COLS) {
                cout << "|";
            }
        }
        cout << endl;
    }
}

inline void initBoard(vector<vector<int>>& board) {
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            board[r][c] = 0;
        }
    }
}

inline int getDropRow(vector<vector<int>>& board, int col) {
    for (int r = NUM_ROWS - 1; r >= 0; r--) {
        if (board[r][col] == 0) return r;
    }
    return -1;
}

inline int getDropRow(vector<int>& heights, int col) {
    return heights[col];
}

inline void initHeights(vector<vector<int>>& board, vector<int>& heights) {
    for (int c = 0; c < NUM_COLS; c++) {
        heights[c] = getDropRow(board, c);
    }
}

inline void makeMove(vector<vector<int>>& board, Move move) {
    if (move.player == 0) {
        for (int r = 0; r < NUM_ROWS; r++) {
            if (board[r][move.col] != 0) {
                board[r][move.col] = 0;
                break;
            }
        }
    } else {
        for (int r = NUM_ROWS - 1; r >= 0; r--) {
            if (board[r][move.col] == 0) {
                board[r][move.col] = move.player;
                break;
            }
        }
    }
}

inline void makeMove(vector<vector<int>>& board, vector<int>& heights, Move move) {
    if (move.player == 0) {
        heights[move.col]++;
        board[heights[move.col]][move.col] = 0;
    } else {
        board[heights[move.col]][move.col] = move.player;
        heights[move.col]--;
    }
}

inline Move userMove(vector<vector<int>>& board) {
    int col;
    cout << "Enter column (0-" << NUM_COLS - 1 << "): ";
    cin >> col;
    if (col < 0 || col >= NUM_COLS) {
        cout << "Invalid column. Try again." << endl;
        return userMove(board);
    }
    if (board[0][col] != 0) {
        cout << "Column is full. Try again." << endl;
        return userMove(board);
    }
    return Move(col, PLAYER);
}

inline bool isWinning(vector<vector<int>>& board, int player) {
    const BitBoardState state = boardToBitBoard(board);
    return isWinning(state, player);
}

inline bool isWinningMove(vector<vector<int>>& board, int player, int row, int col) {
    const BitBoardState state = boardToBitBoard(board);
    return isWinningMove(state, player, row, col);
}

inline int evaluateBoard(vector<vector<int>>& board) {
    const BitBoardState state = boardToBitBoard(board);
    return evaluateBoard(state);
}

inline Move tacticalPrepass(vector<vector<int>>& board) {
    BitBoardState state = boardToBitBoard(board);
    return tacticalPrepass(state);
}

inline Move aiMove(vector<vector<int>>& board) {
    BitBoardState state = boardToBitBoard(board);
    return aiMove(state);
}

inline Move miniMax(vector<vector<int>>& boardCopy, bool isMaximizing, int depth, int alpha = -1000, int beta = 1000, int lastRow = -1, int lastCol = -1, vector<int>* heights = nullptr) {
    (void)lastRow;
    (void)heights;
    BitBoardState state = boardToBitBoard(boardCopy);
    return miniMax(state, isMaximizing, depth, alpha, beta, lastCol);
}

