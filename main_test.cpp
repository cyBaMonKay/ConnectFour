#include <vector>
#include <iostream>
#include "connectfour.h"

using namespace std;


// ============ SIMPLE TEST FRAMEWORK ============

int testsPassed = 0;
int testsFailed = 0;

void assert_equal(int actual, int expected, const string& testName) {
    if (actual == expected) {
        cout << "[PASS] " << testName << endl;
        testsPassed++;
    } else {
        cout << "[FAIL] " << testName << " (expected " << expected << ", got " << actual << ")" << endl;
        testsFailed++;
    }
}

void assert_true(bool condition, const string& testName) {
    if (condition) {
        cout << "[PASS] " << testName << endl;
        testsPassed++;
    } else {
        cout << "[FAIL] " << testName << endl;
        testsFailed++;
    }
}

void assert_false(bool condition, const string& testName) {
    if (!condition) {
        cout << "[PASS] " << testName << endl;
        testsPassed++;
    } else {
        cout << "[FAIL] " << testName << endl;
        testsFailed++;
    }
}

// ============ UNIT TESTS ============

void testInitBoard() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    bool allEmpty = true;
    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS; c++) {
            if (board[r][c] != 0) allEmpty = false;
        }
    }
    assert_true(allEmpty, "InitBoard");
}

void testMakeSingleMove() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    makeMove(board, Move(3, PLAYER));
    assert_equal(board[NUM_ROWS - 1][3], PLAYER, "MakeSingleMove");
}

void testStackMovesInColumn() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    makeMove(board, Move(0, PLAYER));
    makeMove(board, Move(0, COMPUTER));
    makeMove(board, Move(0, PLAYER));
    assert_equal(board[NUM_ROWS - 1][0], PLAYER, "StackMoves_Bottom");
    assert_equal(board[NUM_ROWS - 2][0], COMPUTER, "StackMoves_Middle");
    assert_equal(board[NUM_ROWS - 3][0], PLAYER, "StackMoves_Top");
}

void testUndoMove() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    makeMove(board, Move(2, PLAYER));
    makeMove(board, Move(2, 0));
    assert_equal(board[NUM_ROWS - 1][2], 0, "UndoMove");
}

void testHorizontalWinPlayer() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = PLAYER;
    board[NUM_ROWS - 1][1] = PLAYER;
    board[NUM_ROWS - 1][2] = PLAYER;
    board[NUM_ROWS - 1][3] = PLAYER;
    assert_true(isWinning(board, PLAYER), "HorizontalWin_Player");
    assert_false(isWinning(board, COMPUTER), "HorizontalWin_NotComputer");
}

void testHorizontalWinComputer() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[0][3] = COMPUTER;
    board[0][4] = COMPUTER;
    board[0][5] = COMPUTER;
    board[0][6] = COMPUTER;
    assert_true(isWinning(board, COMPUTER), "HorizontalWin_Computer");
    assert_false(isWinning(board, PLAYER), "HorizontalWin_NotPlayer");
}

void testVerticalWin() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[0][1] = PLAYER;
    board[1][1] = PLAYER;
    board[2][1] = PLAYER;
    board[3][1] = PLAYER;
    assert_true(isWinning(board, PLAYER), "VerticalWin");
}

void testDiagonalWinTopLeftBottomRight() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[2][0] = COMPUTER;
    board[3][1] = COMPUTER;
    board[4][2] = COMPUTER;
    board[5][3] = COMPUTER;
    assert_true(isWinning(board, COMPUTER), "DiagonalWin_TopLeftBottomRight");
}

void testDiagonalWinTopRightBottomLeft() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[2][6] = PLAYER;
    board[3][5] = PLAYER;
    board[4][4] = PLAYER;
    board[5][3] = PLAYER;
    assert_true(isWinning(board, PLAYER), "DiagonalWin_TopRightBottomLeft");
}

void testNoWinWithThreeInRow() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = PLAYER;
    board[NUM_ROWS - 1][1] = PLAYER;
    board[NUM_ROWS - 1][2] = PLAYER;
    assert_false(isWinning(board, PLAYER), "NoWin_ThreeInRow");
}

void testNoWinEmptyBoard() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    assert_false(isWinning(board, PLAYER), "NoWin_EmptyBoard_Player");
    assert_false(isWinning(board, COMPUTER), "NoWin_EmptyBoard_Computer");
}

void testMiniMaxEmptyBoard() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    Move result = miniMax(board, true, 1);
    assert_equal(result.score, 0, "MiniMax_EmptyBoard");
}

void testMiniMaxReturnsValidColumn() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    Move result = miniMax(board, true, 1);
    assert_true(result.col >= 0 && result.col < NUM_COLS, "MiniMax_ReturnsValidColumn");
}

void testMiniMaxTakesImmediateWin() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = COMPUTER;
    board[NUM_ROWS - 1][1] = COMPUTER;
    board[NUM_ROWS - 1][2] = COMPUTER;

    Move result = miniMax(board, true, 1);

    assert_equal(result.col, 3, "MiniMax_TakesImmediateWin_Col");
    assert_equal(result.score, 1000, "MiniMax_TakesImmediateWin_Score");
}

void testMiniMaxBlocksImmediateLoss() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = PLAYER;
    board[NUM_ROWS - 1][1] = PLAYER;
    board[NUM_ROWS - 1][2] = PLAYER;

    Move result = miniMax(board, true, 2);

    assert_equal(result.col, 3, "MiniMax_BlocksImmediateLoss_Col");
}

void testMiniMaxComputerWinning() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = COMPUTER;
    board[NUM_ROWS - 1][1] = COMPUTER;
    board[NUM_ROWS - 1][2] = COMPUTER;
    board[NUM_ROWS - 1][3] = COMPUTER;
    Move result = miniMax(board, true, 1);
    assert_equal(result.score, 1000, "MiniMax_ComputerWinning");
}

void testMiniMaxPlayerWinning() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = PLAYER;
    board[NUM_ROWS - 1][1] = PLAYER;
    board[NUM_ROWS - 1][2] = PLAYER;
    board[NUM_ROWS - 1][3] = PLAYER;
    Move result = miniMax(board, false, 1);
    assert_equal(result.score, -1000, "MiniMax_PlayerWinning");
}

void testMoveStructConstructors() {
    Move m1;
    assert_equal(m1.col, -1, "Move_DefaultConstructor_Col");
    assert_equal(m1.player, 0, "Move_DefaultConstructor_Player");
    Move m2(3, PLAYER);
    assert_equal(m2.col, 3, "Move_ParamConstructor_Col");
    assert_equal(m2.player, PLAYER, "Move_ParamConstructor_Player");
}

int main() {
    cout << "========================================" << endl;
    cout << "   CONNECT FOUR - UNIT TEST SUITE" << endl;
    cout << "========================================" << endl << endl;
    
    testInitBoard();
    testMakeSingleMove();
    testStackMovesInColumn();
    testUndoMove();
    testHorizontalWinPlayer();
    testHorizontalWinComputer();
    testVerticalWin();
    testDiagonalWinTopLeftBottomRight();
    testDiagonalWinTopRightBottomLeft();
    testNoWinWithThreeInRow();
    testNoWinEmptyBoard();
    testMiniMaxEmptyBoard();
    testMiniMaxReturnsValidColumn();
    testMiniMaxTakesImmediateWin();
    testMiniMaxBlocksImmediateLoss();
    testMiniMaxComputerWinning();
    testMiniMaxPlayerWinning();
    testMoveStructConstructors();
    
    cout << endl << "========================================" << endl;
    cout << "TOTAL TESTS: " << (testsPassed + testsFailed) << endl;
    cout << "PASSED: " << testsPassed << endl;
    cout << "FAILED: " << testsFailed << endl;
    cout << "========================================" << endl;
    
    return (testsFailed == 0) ? 0 : 1;
}
