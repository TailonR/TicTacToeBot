//
// Created by renew on 3/9/2021.
//

#include "aiLogic.h"
#include <algorithm>
//////////////////////////////////////////////////////////////// MiniMax ///////////////////////////////////////////////////////////////////////////////////
Minimax::Minimax(int player1Type, int player2Type): player1(player1Type), player2(player2Type) {}

// scorePosition
// places value of the horizontal, vertical, and diagonal rows on the board based on the player type (o or x)
int Minimax::scorePosition(const Board & board, int playerType) {
    {
        int numInRowToWin = 4;
        int score = 0;
        // In bytes
        auto sizeOfString = board.getCell(0, 0).capacity()*sizeof(char) + sizeof(std::string);
        auto sizeOfBoardRow = board.getRow(0).capacity()*sizeOfString+sizeof(std::vector<std::string>);
        auto sizeOfBoard = board.capacity()*sizeOfBoardRow + sizeof(std::vector<std::vector<std::string>>);
//    std::cout << "The size of the board: " << sizeOfBoard << "b" << std::endl;
//    std::cout << std::endl;

        // Score Horizontal
        for(int cIndex = 0; cIndex < board.colSize()-3; cIndex++) {
            for(int rIndex = 0; rIndex < board.rowSize(); rIndex++) {
                std::vector<std::string> column;
                // Get a column in a vector
                for(int increment = 0; increment < numInRowToWin; increment++)
                    column.push_back(board.getCell(rIndex, cIndex + increment));

                score += evaluateBoard(column, playerType);
            }
        }

        // Score Vertical
        for(int rIndex = 0; rIndex < board.rowSize()-3; rIndex++) {
            for(int cIndex = 0; cIndex < board.colSize(); cIndex++) {
                std::vector<std::string> rowAtAColumn;
                // Get all rows in a column in a vector
                for(int increment = 0; increment < numInRowToWin; increment++)
                    rowAtAColumn.push_back(board.getCell(rIndex + increment, cIndex));

                score += evaluateBoard(rowAtAColumn, playerType);
            }
        }

        // Score Diagonal
        //      Score Negative diagonal
        for(int rIndex = 0; rIndex < board.rowSize()-3; rIndex++) {
            for (int cIndex = 0; cIndex < board.colSize() - 3; cIndex++) {
                std::vector<std::string> negDiagonal;
                // Get all squares forming a negatively sloped diagonal in a vector
                for (int i = 0; i < numInRowToWin; i++) {
                    negDiagonal.push_back(board.getCell(rIndex + i, cIndex + i));
                }

                score += evaluateBoard(negDiagonal, playerType);
            }
        }

        //      Score Positive diagonal
        for(int rIndex = 0; rIndex < board.rowSize()-3; rIndex++) {
            for(int cIndex = 3; cIndex < board.colSize(); cIndex++) {
                std::vector<std::string> posDiagonal;
                // Get all squares forming a positively sloped diagonal in a vector
                for(int i = 0; i < numInRowToWin; i++) {
                    posDiagonal.push_back(board.getCell(rIndex + i, cIndex - i));
                }

                score += evaluateBoard(posDiagonal, playerType);
            }
        }
        return score;
    }
}

// evaluateBoard
// The evaluation function
// It evaluates the given places on a board based on the player type (o or x)
//      and returns a value based on how many player markers are already in the given places
//      and how many opponent markers there are in the given places
int Minimax::evaluateBoard(const std::vector<std::string> &collection, int playerType) {
    std::string type = (playerType == 0 ? "o" : "x");
    std::string oppType = (playerType == 0 ? "x" : "o");
    int numInRowToWin = 4;
    int score = 0;

    int playerMarkCount = std::count(collection.begin(),
                                     (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                     type);

    auto blankSpaceCount = std::count(collection.begin(),
                                      (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                      "");


    if (playerMarkCount == 4)
        score += 1000;
    else if (playerMarkCount == 3 && blankSpaceCount == 1)
        score += 500;
    else if (playerMarkCount == 2 && blankSpaceCount == 2)
        score += 400;
    else if (playerMarkCount == 1 && blankSpaceCount == 3)
        score += 300;

    int opponentMarkCount = std::count(collection.begin(),
                                       (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                       oppType);

    blankSpaceCount = std::count(collection.begin(),
                                 (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                 "");

    if (opponentMarkCount == 4)
        score -= 500;
    else if (opponentMarkCount == 3 && blankSpaceCount == 1)
        score -= 750;
    else if (opponentMarkCount == 2 && blankSpaceCount == 2)
        score -= 200;
    else if (opponentMarkCount == 1 && blankSpaceCount == 3)
        score -= 10;

    return score;
}

// checkTerminalNode
// Check if we are in a node where the game is won or where all spaces are filled
bool Minimax::checkTerminalNode(const Board & board) {
    return std::get<0>(board.gameWinningMove()) || board.getValidLocations().empty();
}

// minimax
// determines the behavior of the ai in placing markers
std::pair<std::pair<int,int>, int> Minimax::minimax(const Board &board, int depth, bool maximizingPlayer,
                                                    int playerType, int &boardCount) {
    auto validLocations = board.getValidLocations();
    enum type {o = 0, x = 1, none};
    bool isTerminal = checkTerminalNode(board);
    if (depth == 0 || isTerminal) {
        if (isTerminal) {
            boardCount++;
            if (std::get<0>(board.gameWinningMove()) && maximizingPlayer)
                return std::make_pair(std::make_pair(-1,-1), 1000000000);
            else if (std::get<0>(board.gameWinningMove()) && !maximizingPlayer)
                return std::make_pair(std::make_pair(-1,-1), -1000000000);
            else
                return std::make_pair(std::make_pair(-1,-1), 0);
        }
        else {
            auto score = scorePosition(board, (playerType==0)? o: x);
            boardCount++;
            return std::make_pair(std::make_pair(-1,-1), score);
        }
    }

    if (maximizingPlayer) {
        int score = -1000000000;
        auto bestLocation = validLocations[rand()%validLocations.size()];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType == 0) ? o : x);
            int newScore = std::get<1>(minimax(tempBoard, depth-1, false, (playerType==0)? x: o, boardCount));
            if (newScore > score) {
                score = newScore;
                bestLocation = location;
            }
        }
        return std::make_pair(bestLocation, score);

    } else { //Minimizing player
        int score = 1000000000;
        auto bestLocation = validLocations[rand()%validLocations.size()];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType == 0) ? x : o);
            auto newScore = std::get<1>(minimax(tempBoard, depth-1, true, (playerType==0)? o: x, boardCount));
            if(newScore < score) {
                score = newScore;
                bestLocation = location;
            }
        }
        return std::make_pair(bestLocation, score);
    }
}
/////////////////////////////////////////////////////////////////////// End of Minimax ///////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////// Alpha Beta //////////////////////////////////////////////////////////////////////////////
AlphaBeta::AlphaBeta(int player1Type, int player2Type): player1(player1Type), player2(player2Type) {}

int AlphaBeta::scorePosition(const Board & board, int playerType) {
    {
        int numInRowToWin = 4;
        int score = 0;
        // In bytes
        auto sizeOfString = board.getCell(0, 0).capacity()*sizeof(char) + sizeof(std::string);
        auto sizeOfBoardRow = board.getRow(0).capacity()*sizeOfString+sizeof(std::vector<std::string>);
        auto sizeOfBoard = board.capacity()*sizeOfBoardRow + sizeof(std::vector<std::vector<std::string>>);
//    std::cout << "The size of the board: " << sizeOfBoard << "b" << std::endl;
//    std::cout << std::endl;

        // Score Horizontal
        for(int cIndex = 0; cIndex < board.colSize()-3; cIndex++) {
            for(int rIndex = 0; rIndex < board.rowSize(); rIndex++) {
                std::vector<std::string> column;
                // Get a column in a vector
                for(int increment = 0; increment < numInRowToWin; increment++)
                    column.push_back(board.getCell(rIndex, cIndex + increment));

                score += evaluateBoard(column, playerType);
            }
        }

        // Score Vertical
        for(int rIndex = 0; rIndex < board.rowSize()-3; rIndex++) {
            for(int cIndex = 0; cIndex < board.colSize(); cIndex++) {
                std::vector<std::string> rowAtAColumn;
                // Get all rows in a column in a vector
                for(int increment = 0; increment < numInRowToWin; increment++)
                    rowAtAColumn.push_back(board.getCell(rIndex + increment, cIndex));

                score += evaluateBoard(rowAtAColumn, playerType);
            }
        }

        // Score Diagonal
        //      Score Negative diagonal
        for(int rIndex = 0; rIndex < board.rowSize()-3; rIndex++) {
            for (int cIndex = 0; cIndex < board.colSize() - 3; cIndex++) {
                std::vector<std::string> negDiagonal;
                // Get all squares forming a negatively sloped diagonal in a vector
                for (int i = 0; i < numInRowToWin; i++) {
                    negDiagonal.push_back(board.getCell(rIndex + i, cIndex + i));
                }

                score += evaluateBoard(negDiagonal, playerType);
            }
        }

        //      Score Positive diagonal
        for(int rIndex = 0; rIndex < board.rowSize()-3; rIndex++) {
            for(int cIndex = 3; cIndex < board.colSize(); cIndex++) {
                std::vector<std::string> posDiagonal;
                // Get all squares forming a positively sloped diagonal in a vector
                for(int i = 0; i < numInRowToWin; i++) {
                    posDiagonal.push_back(board.getCell(rIndex + i, cIndex - i));
                }

                score += evaluateBoard(posDiagonal, playerType);
            }
        }
        return score;
    }
}

// evaluateBoard
// The evaluation function
// It evaluates the given places on a board based on the player type (o or x)
//      and returns a value based on how many player markers are already in the given places
//      and how many opponent markers there are in the given places
int AlphaBeta::evaluateBoard(const std::vector<std::string> &collection, int playerType) {
    std::string type = (playerType == 0 ? "o" : "x");
    std::string oppType = (playerType == 0 ? "x" : "o");
    int numInRowToWin = 4;
    int score = 0;

    int playerMarkCount = std::count(collection.begin(),
                                     (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                     type);

    auto blankSpaceCount = std::count(collection.begin(),
                                      (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                      "");


    if (playerMarkCount == 4)
        score += 1000;
    else if (playerMarkCount == 3 && blankSpaceCount == 1)
        score += 500;
    else if (playerMarkCount == 2 && blankSpaceCount == 2)
        score += 400;
    else if (playerMarkCount == 1 && blankSpaceCount == 3)
        score += 300;

    int opponentMarkCount = std::count(collection.begin(),
                                       (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                       oppType);

    blankSpaceCount = std::count(collection.begin(),
                                 (collection.begin() + numInRowToWin != collection.end() ? collection.begin() + numInRowToWin : collection.end()),
                                 "");

    if (opponentMarkCount == 4)
        score -= 500;
    else if (opponentMarkCount == 3 && blankSpaceCount == 1)
        score -= 750;
    else if (opponentMarkCount == 2 && blankSpaceCount == 2)
        score -= 200;
    else if (opponentMarkCount == 1 && blankSpaceCount == 3)
        score -= 10;

    return score;
}

// checkTerminalNode
// Check if we are in a node where the game is won or where all spaces are filled
bool AlphaBeta::checkTerminalNode(const Board & board) {
    return std::get<0>(board.gameWinningMove()) || board.getValidLocations().empty();
}

// alphaBetaMinimax
// determines the behavior of the ai in placing markers
std::pair<std::pair<int, int>, int> AlphaBeta::alphaBetaMinimax(const Board & board, int depth, int alpha, int beta, bool maximizingPlayer, int playerType, int &boardCount) {
    enum type {o = 0, x = 1, none};
    auto validLocations = board.getValidLocations();
    bool isTerminal = checkTerminalNode(board);
    if (depth == 0 || isTerminal) {
        if (isTerminal) {
            boardCount++;
            if (std::get<0>(board.gameWinningMove()) && maximizingPlayer)
                return std::make_pair(std::make_pair(-1,-1), 1000000000);
            else if (std::get<0>(board.gameWinningMove()) && !maximizingPlayer)
                return std::make_pair(std::make_pair(-1,-1), -1000000000);
            else
                return std::make_pair(std::make_pair(-1,-1), 0);
        }
        else {
            auto score = scorePosition(board, (playerType==0)? o: x); //playerType is whoever is x
            boardCount++;
            return std::make_pair(std::make_pair(-1,-1), score); //may need to use player type
        }
    }

    if (maximizingPlayer) {
        int score = -1000000000;
        auto bestLocation = validLocations[rand()%validLocations.size()];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType == 0) ? o : x); //playerType is whoever is x
            int newScore = std::get<1>(alphaBetaMinimax(tempBoard, depth-1, alpha, beta, false, (playerType==0)? x: o, boardCount));
            if (newScore > score) {
                score = newScore;
                bestLocation = location;
            }
            alpha = std::max(alpha, score);
            if(alpha >= beta)
                break;
        }
        return std::make_pair(bestLocation, score);

    } else { //Minimizing player
        int score = 1000000000;
        auto bestLocation = validLocations[rand()%validLocations.size()];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType == 0) ? x : o); //playerType is whoever is o
            int newScore = std::get<1>(alphaBetaMinimax(tempBoard, depth-1, alpha, beta, true, (playerType==0)? o: x, boardCount));
            if(newScore < score) {
                score = newScore;
                bestLocation = location;
            }
            beta = std::min(beta, score);
            if (alpha >= beta)
                break;
        }
        return std::make_pair(bestLocation, score);
    }
}
