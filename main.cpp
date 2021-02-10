// main.cpp
// Author: Tailon Russell
// The purpose of this file is to contain the main
//      program to start a tic tac toe game
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <queue>

//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include "doctest.h"

// Player
struct Player {
    explicit Player(int type): playerType(type) {}
    int playerType;
};

// Board
struct Board {
    Board(int row, int col): board(row, std::vector<std::string>(col, "")) {
        createBoard(row, col);
    };
    void createBoard(int row, int col) {

        for(int rIndex = 0; rIndex < row; rIndex++)
            for(int cIndex = 0; cIndex < col; cIndex++)
                board[rIndex][cIndex] = "";
    };
    std::vector<std::vector<std::string>> board;
};

struct TurnOutcome {
    TurnOutcome(int d, int s, int bpps, bool p): depth(d), step(s), boardProcessingSpeed(bpps), pruning(p){};
    int depth;
    int step;
    int boardProcessingSpeed;
    bool pruning;
};

bool operator<(const TurnOutcome &lhs, const TurnOutcome &rhs) {
//    std::cout << "lhs step: " << lhs.step << " rhs step: " << rhs.step << std::endl;
//    std::cout << "lhs depth: " << lhs.depth << " rhs step: " << rhs.depth << std::endl;
//    std::cout << "The result: " << (lhs.step > rhs.step || lhs.depth > rhs.depth);
    if(lhs.step > rhs.step)
        return true;
    else if (lhs.step == rhs.step && lhs.depth > rhs.depth)
        return true;
    return false;

}

// makeMove
// inserts the player symbol on the board in the given location
void makeMove(std::vector<std::vector<std::string>> & theBoard, int row, int col, int playerType){
    if (playerType) {
        theBoard[row][col] = "x";
    } else if(playerType == 0) {
        theBoard[row][col] = "o";
    }
}

// clear Data
// clears the game data
void clearData() {
    std::ofstream open("gameData.txt");
    open.close();
}

// storeMove
// stores the gameplay
void storeMove(const std::vector<std::vector<std::string>> & theBoard){
    std::ofstream saver("gameData.txt", std::ios::app);

    for(auto & row: theBoard){
        for(auto & element: row)
            if(!element.empty())
                saver << element << " ";
            else
                saver << "_" << " ";
        saver << std::endl;
    }
    saver.close();
}

// loadGame
// stores the gameplay
void loadGame(){
    std::ifstream loader("gameData.txt");
    std::string line;
    int rowNumber = 0;
    while(std::getline(loader, line)) {
        if (line.substr(0,4) == "Turn")
            std::cout << "\n" << line << "\n" << std::endl;
        else
            std::cout << line << std::endl;

        rowNumber++;
        if(rowNumber == 5)
        {
            std::cout << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            rowNumber = 0;
        }
    }

}

// operator<<
// auxiliary function to print the board (for gui)
std::ostream& operator<< (std::ostream& os,  const std::vector<std::vector<std::string>> & board)
{
    for (auto & row: board) {
        for(auto  & square: row)
            os << (!square.empty()? square: "_") << " ";
        os << "\n";
    }
    return os;
}

// validLocation
// Checks if the place specified by the given row and column is a valid space
//      meaning if it's on the board and if there is no other marker that's already there
bool validLocation(int row, int col, const std::vector<std::vector<std::string>> & board) {
    if(row < board.size() && col < board[0].size() && board[row][col].empty())
        return true;
    return false;
}

// gameWinningMove
// Checks if there is a win on the board
// What's left:
//      Think about return a pair <bool, int> the int is the player type that won
std::pair<bool, int> gameWinningMove(const std::vector<std::vector<std::string>> & board) {
    enum type {o = 0, x = 1, none};
    // Check Horizontal win
    for(int rowIndex = 0; rowIndex < board.size(); rowIndex++) {
        for(int colIndex = 0; colIndex < board[0].size()-3; colIndex++) {
            if(!board[rowIndex][colIndex].empty() && board[rowIndex][colIndex+1] == board[rowIndex][colIndex] &&
                    board[rowIndex][colIndex+2] == board[rowIndex][colIndex] && board[rowIndex][colIndex+3] == board[rowIndex][colIndex])
                return std::make_pair(true, (board[rowIndex][colIndex] == "o"? o: x));
        }
    }

    // Check Vertical win
    for(int rowIndex = 0; rowIndex < board.size()-3; rowIndex++) {
        for(int colIndex = 0; colIndex < board[0].size(); colIndex++) {
            if(!board[rowIndex][colIndex].empty() && board[rowIndex+1][colIndex] == board[rowIndex][colIndex] &&
                    board[rowIndex+2][colIndex] == board[rowIndex][colIndex] && board[rowIndex+3][colIndex] == board[rowIndex][colIndex])
                return std::make_pair(true, (board[rowIndex][colIndex] == "o"? o: x));
        }
    }

    // Check Diagonal win
    for(int rowIndex = 0; rowIndex < board.size()-3; rowIndex++) {
        for(int colIndex = 0; colIndex < board[0].size()-3; colIndex++) {
            if(!board[rowIndex][colIndex].empty() && board[rowIndex+1][colIndex+1] == board[rowIndex][colIndex] &&
               board[rowIndex+2][colIndex+2] == board[rowIndex][colIndex] && board[rowIndex+3][colIndex+3] == board[rowIndex][colIndex])
                return std::make_pair(true, (board[rowIndex][colIndex] == "o"? o: x));
        }

        for(int colIndex = 3; colIndex < board[0].size(); colIndex++) {
            if(!board[rowIndex][colIndex].empty() && board[rowIndex+1][colIndex-1] == board[rowIndex][colIndex] &&
               board[rowIndex+2][colIndex-2] == board[rowIndex][colIndex] && board[rowIndex+3][colIndex-3] == board[rowIndex][colIndex])
                return std::make_pair(true, (board[rowIndex][colIndex] == "o"? o: x));
        }
    }

    // Not a winning move
    return std::make_pair(false, none);
}

// evaluateBoard
// The evaluation function
// It evaluates the given places on a board based on the player type (o or x)
//      and returns a value based on how many player markers are already in the given places
//      and how many opponent markers there are in the given places
int evaluateBoard(const std::vector<std::string> & collection, int playerType) {
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

// scorePosition
// places value of the horizontal, vertical, and diagonal rows on the board based on the player type (o or x)
int scorePosition(const std::vector<std::vector<std::string>> & board, int playerType) {
    static int boardCount;
    int numInRowToWin = 4;
    int score = 0;
    // In bytes
    auto sizeOfString = board[0][0].capacity()*sizeof(char) + sizeof(std::string);
    auto sizeOfBoardRow = board[0].capacity()*sizeOfString+sizeof(std::vector<std::string>);
    auto sizeOfBoard = board.capacity()*sizeOfBoardRow + sizeof(std::vector<std::vector<std::string>>);
//    std::cout << "The size of the board: " << sizeOfBoard << "b" << std::endl;
//    std::cout << std::endl;

    // Score Horizontal
    for(int cIndex = 0; cIndex < board[0].size()-3; cIndex++) {
        for(auto & rIndex : board) {
            std::vector<std::string> column;
            // Get a column in a vector
            for(int increment = 0; increment < numInRowToWin; increment++)
                column.push_back(rIndex[cIndex + increment]);

            score += evaluateBoard(column, playerType);
        }
    }

    // Score Vertical
    for(int rIndex = 0; rIndex < board.size()-3; rIndex++) {
        for(int cIndex = 0; cIndex < board[0].size(); cIndex++) {
            std::vector<std::string> rowAtAColumn;
            // Get all rows in a column in a vector
            for(int increment = 0; increment < numInRowToWin; increment++)
                rowAtAColumn.push_back(board[rIndex + increment][cIndex]);

            score += evaluateBoard(rowAtAColumn, playerType);
        }
    }

    // Score Diagonal
    //      Score Negative diagonal
    for(int rIndex = 0; rIndex < board.size()-3; rIndex++) {
        for (int cIndex = 0; cIndex < board[0].size() - 3; cIndex++) {
            std::vector<std::string> negDiagonal;
            // Get all squares forming a negatively sloped diagonal in a vector
            for (int i = 0; i < numInRowToWin; i++) {
                negDiagonal.push_back(board[rIndex + i][cIndex + i]);
            }

            score += evaluateBoard(negDiagonal, playerType);
        }
    }

    //      Score Positive diagonal
    for(int rIndex = 0; rIndex < board.size()-3; rIndex++) {
        for(int cIndex = 3; cIndex < board[0].size(); cIndex++) {
            std::vector<std::string> posDiagonal;
            // Get all squares forming a positively sloped diagonal in a vector
            for(int i = 0; i < numInRowToWin; i++) {
                posDiagonal.push_back(board[rIndex + i][cIndex - i]);
            }

            score += evaluateBoard(posDiagonal, playerType);
        }
    }
    boardCount++;
//    std::cout << boardCount << std::endl;
    return score;
}

// getValidLocations
// returns a vector of all locations that are valid (as determined by the function validLocation)
std::vector<std::pair<int, int>> getValidLocations(const std::vector<std::vector<std::string>> & board) {
    std::vector<std::pair<int, int>> validLocations;
    for(int rIndex = 0; rIndex < board.size(); rIndex++)
        for(int cIndex = 0; cIndex < board[0].size(); cIndex++)
            if (validLocation(rIndex, cIndex, board))
                validLocations.emplace_back(rIndex, cIndex);
    return validLocations;
}

// checkTerminalNode
// Check if we are in a node where the game is won or where all spaces are filled
bool checkTerminalNode(const std::vector<std::vector<std::string>> & board) {
    return std::get<0>(gameWinningMove(board)) || getValidLocations(board).empty();
}

// minimax
// determines the behavior of the ai in placing markers
std::pair<std::pair<int, int>, int> minimax(const std::vector<std::vector<std::string>> & board, int depth, bool maximizingPlayer, int playerType, int &boardCount) {
    auto validLocations = getValidLocations(board);
    enum type {o = 0, x = 1, none};
    bool isTerminal = checkTerminalNode(board);
    if (depth == 0 || isTerminal) {
        if (isTerminal) {
            boardCount++;
            if (std::get<0>(gameWinningMove(board)) && maximizingPlayer)
                return std::make_pair(std::make_pair(-1,-1), 1000000000);
            else if (std::get<0>(gameWinningMove(board)) && !maximizingPlayer)
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
            makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType==0)? o: x);
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
            makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType==0)? x: o);
            auto newScore = std::get<1>(minimax(tempBoard, depth-1, true, (playerType==0)? o: x, boardCount));
            if(newScore < score) {
                score = newScore;
                bestLocation = location;
            }
        }
        return std::make_pair(bestLocation, score);
    }
}

// alphaBetaMinimax
// determines the behavior of the ai in placing markers
std::pair<std::pair<int, int>, int> alphaBetaMinimax(const std::vector<std::vector<std::string>> & board, int depth, int alpha, int beta, bool maximizingPlayer, int playerType, int &boardCount) {
    enum type {o = 0, x = 1, none};
    auto validLocations = getValidLocations(board);
    bool isTerminal = checkTerminalNode(board);
    if (depth == 0 || isTerminal) {
        if (isTerminal) {
            boardCount++;
            if (std::get<0>(gameWinningMove(board)) && maximizingPlayer)
                return std::make_pair(std::make_pair(-1,-1), 1000000000);
            else if (std::get<0>(gameWinningMove(board)) && !maximizingPlayer)
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
            makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType==0)? o: x); //playerType is whoever is x
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
            makeMove(tempBoard, std::get<0>(location), std::get<1>(location), (playerType==0)? x: o); //playerType is whoever is o
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

// playGame
// This is the function for the gameplay
std::tuple<bool, int, std::priority_queue<TurnOutcome>> playGame(int save, int player1Depth, int player2Depth, int playGame, bool pruning) {
    Board board(5,5); // 5x5 board
    enum type {o = 0, x = 1, none};
    Player player1(o);
    Player player2(x);

    std::tuple<bool, int, std::priority_queue<TurnOutcome>> gameWon;
    std::priority_queue<TurnOutcome> turnOutcomes;
    bool tie = false;

    bool saveGame = save;
    bool repeat = false;
    int turn = o;
    int turnCount = 0;
    int userRow;
    int userCol;
    int boardCount = 0;
    std::priority_queue<TurnOutcome> testCout;
    // Clear the saved game data
    clearData();
    // Here we want to loop until the game is won; this is the bulk of the gameplay
    while (!std::get<0>(gameWon) && !tie) {
        repeat = false;
        if(turn == o) {
            turnCount++;
            boardCount = 0;
            if (playGame) {
                std::cout << "Enter a row number (0-" << board.board.size()-1 << "): " << std::endl;
                std::cin >> userRow;
                std::cout << "Enter a column number (0-" << board.board[0].size()-1 << "): " << std::endl;
                std::cin >> userCol;
                if(validLocation(userRow, userCol, board.board))
                    makeMove(board.board, userRow, userCol, player1.playerType);
                else
                    repeat = true;
            } else {
                if(player1Depth == 0) {
                    std::srand(time(0));
                    int row = std::rand()%board.board.size();
                    int col = std::rand()%board.board[0].size();
                    if(validLocation(row, col, board.board))
                        makeMove(board.board, row, col, player1.playerType);
                    else
                        repeat = true;

               } else {
                    clock_t start = clock();
                    auto player1Move = (pruning)? std::get<0>(alphaBetaMinimax(board.board, player1Depth, -1000000000, 1000000000, true, player1.playerType, boardCount)):
                                       std::get<0>(minimax(board.board, player1Depth, true, player1.playerType, boardCount));
                    clock_t end = clock();
                    auto timeSpent = (end-start)/CLOCKS_PER_SEC;
                    makeMove(board.board, std::get<0>(player1Move), std::get<1>(player1Move), player1.playerType);
                    int boardProcessingSpeed = (timeSpent>0? boardCount/timeSpent: boardCount);
                    auto turnInfo = TurnOutcome(player1Depth, turnCount, boardProcessingSpeed, pruning);
                    turnOutcomes.push(turnInfo);
//                    std::cout << "The number of boards processed(bp): " << boardCount << " and number of board processed per second (bpps): " <<  (timeSpent>0? boardCount/timeSpent: boardCount) << "bpps" << std::endl;
                }
            }
        }
        else {
            turnCount++;
            if (player2Depth == 0) {
                int row = std::rand()%board.board.size();
                int col = std::rand()%board.board[0].size();
                if(validLocation(row, col, board.board)) {
                    makeMove(board.board, row, col, player2.playerType);
                }
                else
                    repeat = true;
            } else {
                boardCount = 0;
                clock_t start = clock();
                auto player2Move = (pruning)? std::get<0>(alphaBetaMinimax(board.board, player2Depth, -1000000000, 1000000000, true, player2.playerType, boardCount)):
                                   std::get<0>(minimax(board.board, player2Depth, true, player2.playerType, boardCount));
                clock_t end = clock();
                auto timeSpent = (end-start)/CLOCKS_PER_SEC;
                makeMove(board.board, std::get<0>(player2Move), std::get<1>(player2Move), player2.playerType);
                int boardProcessingSpeed = (timeSpent>0? boardCount/timeSpent: boardCount);
                auto turnInfo = TurnOutcome(player2Depth, turnCount, boardProcessingSpeed, pruning);
                turnOutcomes.push(turnInfo);
//                std::cout << "The number of boards processed(bp): " << boardCount << " and number of board processed per second (bpps): " << (timeSpent>0? boardCount/timeSpent: boardCount) << "bpps" << std::endl;
            }
        }
        if(!repeat)
            turn = !turn;

//        std::cout << board.board;
//        std::cout << std::endl;

        gameWon = std::make_tuple(std::get<0>(gameWinningMove(board.board)), std::get<1>(gameWinningMove(board.board)), turnOutcomes);
        if(getValidLocations(board.board).empty())
            tie = true;

        if(saveGame)
            storeMove(board.board);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return gameWon;
}

std::ostream& operator<< (std::ostream& os, std::priority_queue<TurnOutcome> boardProcessingData) {

    while(!boardProcessingData.empty()) {
        os << boardProcessingData.top().step << " | " << boardProcessingData.top().depth << " | " << boardProcessingData.top().boardProcessingSpeed << std::endl;
        boardProcessingData.pop();
    }
    return os;
}

// main
// This is the main function for the gameplay
int main() {
    enum type {o = 0, x = 1, none};
//    int oWinCount = 0;
//    int xWinCount = 0;
//    int tieCount = 0;
//    for(int player1Depth = 1; player1Depth < 5; player1Depth++) {
//        for(int player2Depth = 1; player2Depth < 5; player2Depth++) {
//            std::cout << "player 1 depth: " << player1Depth << " player 2 depth: " << player2Depth << std::endl;
//            auto gameOutcome = playGame(false, player1Depth, player2Depth, 0); // Going to change to be dependant on user input
//            if(std::get<0>(gameOutcome)) {
//                if(std::get<1>(gameOutcome) == o) {
//                    oWinCount++;
//                }
//                else if (std::get<1>(gameOutcome) == x) {
//                    xWinCount++;
//                }
//            } else {
//                tieCount++;
//            }
//        }
//    }
//
//    std::cout << "o won " << oWinCount << " times" << std::endl;
//    std::cout << "x won " << xWinCount << " times" << std::endl;
//    std::cout << tieCount << " ties" << std::endl;

    int saveTheGame = 0;
    int pruning = 0;
    int recapGame = 0;
    int playTheGame = 0;
    int player1Depth = 1;
    int player2Depth = 1;
    std::priority_queue<TurnOutcome> boardProcessingSpeedsMinimax;
    std::priority_queue<TurnOutcome> boardProcessingSpeedsAlphaBeta;
    for(int playerDepth = 1; playerDepth < 5; playerDepth++) {
        auto boardProcessingSpeedData = std::get<2>(playGame(saveTheGame, playerDepth, playerDepth, playTheGame, pruning));
        while (!boardProcessingSpeedData.empty()) {
            boardProcessingSpeedsMinimax.push(boardProcessingSpeedData.top());
            boardProcessingSpeedData.pop();
        }
    }
    std::cout << "Step | Depth | BPPS w/ minimax" << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << boardProcessingSpeedsMinimax;
    std::cout << std::endl;
    pruning = 1;
    for(int playerDepth = 1; playerDepth < 5; playerDepth++) {
        auto boardProcessingSpeedData = std::get<2>(playGame(saveTheGame, playerDepth, playerDepth, playTheGame, pruning));
        while (!boardProcessingSpeedData.empty()) {
            boardProcessingSpeedsAlphaBeta.push(boardProcessingSpeedData.top());
            boardProcessingSpeedData.pop();
        }
    }
    std::cout << "Step | Depth | Bpps w/ alpha-beta" << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;
    std::cout << boardProcessingSpeedsAlphaBeta;
//    std::cout << "Watch game or play game (0 = watch  1 = play)" << std::endl;
//    std::cin >> playTheGame;
//
//    std::cout << "Do you want to save this game (0 = no  1 = yes)" << std::endl;
//    std::cin >> saveTheGame;
//
//    std::cout << "Do you want pruning activated (0 = no  1 = yes)" << std::endl;
//    std::cin >> pruning;

   if(playTheGame) {
//       std::cout << "Select difficulty" << std::endl;
//       std::cout << "0) Very Easy\n1) Easy\n2) Challenging\n3) Difficult\n4) Hard\n5) Very Hard" << std::endl;
//       std::cin >> player2Depth;
   } else {
//       std::cout << "Select bot 1 expert level" << std::endl;
//       std::cout << "0) Dumb\n1) Novice\n2) Intermediate\n3) Skilled\n4) Expert\n5) Genius" << std::endl;
//       std::cin >> player1Depth;
//       std::cout << "Select bot 2 expert level" << std::endl;
//       std::cout << "0) Dumb \n1) Novice\n2) Intermediate\n3) Skilled\n4) Expert\n5) Genius" << std::endl;
//       std::cin >> player2Depth;
   }

//    auto gameOutput = playGame(saveTheGame, player1Depth, player2Depth, playTheGame, pruning);
//
//    if(!playTheGame) {
//        if(std::get<0>(gameOutput)) {
//            std::cout << ((std::get<1>(gameOutput) == o)? "o": "x") << " won" << std::endl;
//        } else
//                std::cout << "It's a tie" << std::endl;
//    } else if(playTheGame) {
//        if(std::get<0>(gameOutput)) {
//            std::cout << "You won" << std::endl;
//        } else
//            std::cout << "x won" << std::endl;
//    }

    if(saveTheGame) {
        std::cout << "Do you want a game recap (0 = no 1 = yes)" << std::endl;
        std::cin >> recapGame;
        if (recapGame)
            loadGame();
    }
    return 0;
}
