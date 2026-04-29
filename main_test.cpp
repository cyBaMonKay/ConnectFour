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

void testEvaluateWindowComputerThreeAndOneEmpty() {
    vector<int> window = {COMPUTER, COMPUTER, COMPUTER, 0};
    assert_equal(evaluateWindow(window), 5, "EvaluateWindow_ComputerThreeOneEmpty");
}

void testEvaluateWindowComputerTwoAndTwoEmpty() {
    vector<int> window = {COMPUTER, COMPUTER, 0, 0};
    assert_equal(evaluateWindow(window), 2, "EvaluateWindow_ComputerTwoTwoEmpty");
}

void testEvaluateWindowPlayerThreeAndOneEmpty() {
    vector<int> window = {PLAYER, PLAYER, PLAYER, 0};
    assert_equal(evaluateWindow(window), -5, "EvaluateWindow_PlayerThreeOneEmpty");
}

void testEvaluateWindowPlayerTwoAndTwoEmpty() {
    vector<int> window = {PLAYER, PLAYER, 0, 0};
    assert_equal(evaluateWindow(window), -2, "EvaluateWindow_PlayerTwoTwoEmpty");
}

void testEvaluateWindowMixedNoScore() {
    vector<int> window = {COMPUTER, PLAYER, 0, 0};
    assert_equal(evaluateWindow(window), 0, "EvaluateWindow_MixedNoScore");
}

void testEvaluateBoardEmpty() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    assert_equal(evaluateBoard(board), 0, "EvaluateBoard_Empty");
}

void testEvaluateBoardComputerWinning() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = COMPUTER;
    board[NUM_ROWS - 1][1] = COMPUTER;
    board[NUM_ROWS - 1][2] = COMPUTER;
    board[NUM_ROWS - 1][3] = COMPUTER;
    assert_equal(evaluateBoard(board), 1000, "EvaluateBoard_ComputerWinning");
}

void testEvaluateBoardPlayerWinning() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = PLAYER;
    board[NUM_ROWS - 1][1] = PLAYER;
    board[NUM_ROWS - 1][2] = PLAYER;
    board[NUM_ROWS - 1][3] = PLAYER;
    assert_equal(evaluateBoard(board), -1000, "EvaluateBoard_PlayerWinning");
}

void testEvaluateBoardCenterPreferenceComputer() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    vector<vector<int>> edgeBoard(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    initBoard(edgeBoard);
    board[NUM_ROWS - 1][NUM_COLS / 2] = COMPUTER;
    edgeBoard[NUM_ROWS - 1][0] = COMPUTER;

    int centerScore = evaluateBoard(board);
    int edgeScore = evaluateBoard(edgeBoard);

    assert_true(centerScore > edgeScore, "EvaluateBoard_CenterPreferenceComputer_Relative");
    assert_true(centerScore > 0, "EvaluateBoard_CenterPreferenceComputer_Positive");
}

void testEvaluateBoardCenterPreferencePlayer() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    vector<vector<int>> edgeBoard(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    initBoard(edgeBoard);
    board[NUM_ROWS - 1][NUM_COLS / 2] = PLAYER;
    edgeBoard[NUM_ROWS - 1][0] = PLAYER;

    int centerScore = evaluateBoard(board);
    int edgeScore = evaluateBoard(edgeBoard);

    assert_true(centerScore < edgeScore, "EvaluateBoard_CenterPreferencePlayer_Relative");
    assert_true(centerScore < 0, "EvaluateBoard_CenterPreferencePlayer_Negative");
}

void testEvaluateBoardComputerHorizontalThreat() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = COMPUTER;
    board[NUM_ROWS - 1][1] = COMPUTER;
    board[NUM_ROWS - 1][2] = COMPUTER;

    int score = evaluateBoard(board);
    assert_true(score > 0, "EvaluateBoard_ComputerHorizontalThreat_Positive");
    assert_true(score < 1000, "EvaluateBoard_ComputerHorizontalThreat_NonTerminal");
}

void testEvaluateBoardPlayerHorizontalThreat() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = PLAYER;
    board[NUM_ROWS - 1][1] = PLAYER;
    board[NUM_ROWS - 1][2] = PLAYER;

    int score = evaluateBoard(board);
    assert_true(score < 0, "EvaluateBoard_PlayerHorizontalThreat_Negative");
    assert_true(score > -1000, "EvaluateBoard_PlayerHorizontalThreat_NonTerminal");
}

void testEvaluateBoardComputerVerticalThreat() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = COMPUTER;
    board[NUM_ROWS - 2][0] = COMPUTER;
    board[NUM_ROWS - 3][0] = COMPUTER;

    int score = evaluateBoard(board);
    assert_true(score > 0, "EvaluateBoard_ComputerVerticalThreat_Positive");
    assert_true(score < 1000, "EvaluateBoard_ComputerVerticalThreat_NonTerminal");
}

void testEvaluateBoardPlayerVerticalThreat() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS - 1][0] = PLAYER;
    board[NUM_ROWS - 2][0] = PLAYER;
    board[NUM_ROWS - 3][0] = PLAYER;

    int score = evaluateBoard(board);
    assert_true(score < 0, "EvaluateBoard_PlayerVerticalThreat_Negative");
    assert_true(score > -1000, "EvaluateBoard_PlayerVerticalThreat_NonTerminal");
}

void testEvaluateBoardComputerDiagonalThreat() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    // Gravity-valid diagonal threat: support elevated tokens in columns 1 and 2.
    board[NUM_ROWS - 1][0] = COMPUTER;
    board[NUM_ROWS - 1][1] = PLAYER;
    board[NUM_ROWS - 2][1] = COMPUTER;
    board[NUM_ROWS - 1][2] = PLAYER;
    board[NUM_ROWS - 2][2] = PLAYER;
    board[NUM_ROWS - 3][2] = COMPUTER;

    int score = evaluateBoard(board);
    assert_true(score > 0, "EvaluateBoard_ComputerDiagonalThreat_Positive");
    assert_true(score < 1000, "EvaluateBoard_ComputerDiagonalThreat_NonTerminal");
}

void testEvaluateBoardPlayerDiagonalThreat() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    // Gravity-valid diagonal threat: support elevated tokens in columns 5 and 4.
    board[NUM_ROWS - 1][6] = PLAYER;
    board[NUM_ROWS - 1][5] = COMPUTER;
    board[NUM_ROWS - 2][5] = PLAYER;
    board[NUM_ROWS - 1][4] = COMPUTER;
    board[NUM_ROWS - 2][4] = COMPUTER;
    board[NUM_ROWS - 3][4] = PLAYER;

    int score = evaluateBoard(board);
    assert_true(score < 0, "EvaluateBoard_PlayerDiagonalThreat_Negative");
    assert_true(score > -1000, "EvaluateBoard_PlayerDiagonalThreat_NonTerminal");
}

void testMiniMaxEmptyBoard() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    Move result = miniMax(board, true, 0);
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

void testIsWinningMove_Horizontal() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS-1][0] = PLAYER;
    board[NUM_ROWS-1][1] = PLAYER;
    board[NUM_ROWS-1][2] = PLAYER;
    board[NUM_ROWS-1][3] = PLAYER;
    assert_true(isWinningMove(board, PLAYER, NUM_ROWS-1, 3), "IsWinningMove_Horizontal_LastCell");
    assert_true(isWinningMove(board, PLAYER, NUM_ROWS-1, 0), "IsWinningMove_Horizontal_FirstCell");
    assert_false(isWinningMove(board, COMPUTER, NUM_ROWS-1, 3), "IsWinningMove_Horizontal_WrongPlayer");
}

void testIsWinningMove_Vertical() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[0][1] = PLAYER;
    board[1][1] = PLAYER;
    board[2][1] = PLAYER;
    board[3][1] = PLAYER;
    assert_true(isWinningMove(board, PLAYER, 3, 1), "IsWinningMove_Vertical_Bottom");
    assert_true(isWinningMove(board, PLAYER, 0, 1), "IsWinningMove_Vertical_Top");
    assert_false(isWinningMove(board, COMPUTER, 3, 1), "IsWinningMove_Vertical_WrongPlayer");
}

void testIsWinningMove_DiagonalDownRight() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[2][0] = COMPUTER;
    board[3][1] = COMPUTER;
    board[4][2] = COMPUTER;
    board[5][3] = COMPUTER;
    assert_true(isWinningMove(board, COMPUTER, 5, 3), "IsWinningMove_DiagonalDownRight_LastCell");
    assert_true(isWinningMove(board, COMPUTER, 2, 0), "IsWinningMove_DiagonalDownRight_FirstCell");
}

void testIsWinningMove_DiagonalDownLeft() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[2][6] = PLAYER;
    board[3][5] = PLAYER;
    board[4][4] = PLAYER;
    board[5][3] = PLAYER;
    assert_true(isWinningMove(board, PLAYER, 5, 3), "IsWinningMove_DiagonalDownLeft_LastCell");
    assert_true(isWinningMove(board, PLAYER, 2, 6), "IsWinningMove_DiagonalDownLeft_FirstCell");
}

void testIsWinningMove_NoWin() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    board[NUM_ROWS-1][0] = PLAYER;
    board[NUM_ROWS-1][1] = PLAYER;
    board[NUM_ROWS-1][2] = PLAYER;
    assert_false(isWinningMove(board, PLAYER, NUM_ROWS-1, 2), "IsWinningMove_NoWin_ThreeInRow");
    assert_false(isWinningMove(board, PLAYER, NUM_ROWS-1, 0), "IsWinningMove_NoWin_EmptyNeighbor");
}

void testGetDropRow() {
    vector<vector<int>> board(NUM_ROWS, vector<int>(NUM_COLS, 0));
    initBoard(board);
    assert_equal(getDropRow(board, 0), NUM_ROWS - 1, "GetDropRow_EmptyColumn");
    makeMove(board, Move(0, PLAYER));
    assert_equal(getDropRow(board, 0), NUM_ROWS - 2, "GetDropRow_OneInColumn");
    // Fill column 1 completely
    for (int i = 0; i < NUM_ROWS; i++) makeMove(board, Move(1, PLAYER));
    assert_equal(getDropRow(board, 1), -1, "GetDropRow_FullColumn");
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
    testEvaluateWindowComputerThreeAndOneEmpty();
    testEvaluateWindowComputerTwoAndTwoEmpty();
    testEvaluateWindowPlayerThreeAndOneEmpty();
    testEvaluateWindowPlayerTwoAndTwoEmpty();
    testEvaluateWindowMixedNoScore();
    testEvaluateBoardEmpty();
    testEvaluateBoardComputerWinning();
    testEvaluateBoardPlayerWinning();
    testEvaluateBoardCenterPreferenceComputer();
    testEvaluateBoardCenterPreferencePlayer();
    testEvaluateBoardComputerHorizontalThreat();
    testEvaluateBoardPlayerHorizontalThreat();
    testEvaluateBoardComputerVerticalThreat();
    testEvaluateBoardPlayerVerticalThreat();
    testEvaluateBoardComputerDiagonalThreat();
    testEvaluateBoardPlayerDiagonalThreat();
    testMiniMaxEmptyBoard();
    testMiniMaxReturnsValidColumn();
    testMiniMaxTakesImmediateWin();
    testMiniMaxBlocksImmediateLoss();
    testMiniMaxComputerWinning();
    testMiniMaxPlayerWinning();
    testMoveStructConstructors();
    testIsWinningMove_Horizontal();
    testIsWinningMove_Vertical();
    testIsWinningMove_DiagonalDownRight();
    testIsWinningMove_DiagonalDownLeft();
    testIsWinningMove_NoWin();
    testGetDropRow();

    cout << endl << "========================================" << endl;
    cout << "TOTAL TESTS: " << (testsPassed + testsFailed) << endl;
    cout << "PASSED: " << testsPassed << endl;
    cout << "FAILED: " << testsFailed << endl;
    cout << "========================================" << endl;

    return (testsFailed == 0) ? 0 : 1;
}
