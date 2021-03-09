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
#include "player.h"
#include "board.h"
#include "turnOutcome.h"
#include "aiLogic.h"
#include "dataManager.h"

// playGame
// This is the function for the gameplay
std::tuple<bool, int, std::priority_queue<TurnOutcome>> playGame(int save, int player1Depth, int player2Depth, int playGame, bool pruning) {
    Board board(5,5); // 5x5 board
    enum type {o = 0, x = 1, none};
    Player player1(o);
    Player player2(x);
    Minimax minimax (player1.getPlayerType(), player2.getPlayerType());
    AlphaBeta alphaBeta (player1.getPlayerType(), player2.getPlayerType());
    DataManager dataManager;

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
    dataManager.clearData();
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
                if(board.validLocation(userRow, userCol))
                    player1.makeMove(board, userRow, userCol, player1.getPlayerType());
                else
                    repeat = true;
            } else {
                if(player1Depth == 0) {
                    std::srand(time(0));
                    int row = std::rand()%board.board.size();
                    int col = std::rand()%board.board[0].size();
                    if(board.validLocation(row, col))
                        player1.makeMove(board, row, col, player1.getPlayerType());
                    else
                        repeat = true;

               } else {
                    clock_t start = clock();
                    auto player1Move = (pruning)? std::get<0>(alphaBeta.alphaBetaMinimax(board, player1Depth, -1000000000, 1000000000, true, player1.getPlayerType(), boardCount)):
                                       std::get<0>(minimax.minimax(board, player1Depth, true, player1.getPlayerType(), boardCount));
                    clock_t end = clock();
                    auto timeSpent = (end-start)/CLOCKS_PER_SEC;
                    player1.makeMove(board, std::get<0>(player1Move), std::get<1>(player1Move), player1.getPlayerType());
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
                if(board.validLocation(row, col)) {
                    player2.makeMove(board, row, col, player2.getPlayerType());
                }
                else
                    repeat = true;
            } else {
                boardCount = 0;
                clock_t start = clock();
                auto player2Move = (pruning)? std::get<0>(alphaBeta.alphaBetaMinimax(board, player2Depth, -1000000000, 1000000000, true, player2.getPlayerType(), boardCount)):
                                   std::get<0>(minimax.minimax(board, player2Depth, true, player2.getPlayerType(), boardCount));
                clock_t end = clock();
                auto timeSpent = (end-start)/CLOCKS_PER_SEC;
                player2.makeMove(board, std::get<0>(player2Move), std::get<1>(player2Move), player2.getPlayerType());
                int boardProcessingSpeed = (timeSpent>0? boardCount/timeSpent: boardCount);
                auto turnInfo = TurnOutcome(player2Depth, turnCount, boardProcessingSpeed, pruning);
                turnOutcomes.push(turnInfo);
//                std::cout << "The number of boards processed(bp): " << boardCount << " and number of board processed per second (bpps): " << (timeSpent>0? boardCount/timeSpent: boardCount) << "bpps" << std::endl;
            }
        }
        if(!repeat)
            turn = !turn;

        std::cout << board.board;
        std::cout << std::endl;

        gameWon = std::make_tuple(std::get<0>(board.gameWinningMove()), std::get<1>(board.gameWinningMove()), turnOutcomes);
        if(board.getValidLocations().empty())
            tie = true;

        if(saveGame)
            dataManager.storeMove(board);
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
    int saveTheGame = 0;
    int pruning = 0;
    int recapGame = 0;
    int playTheGame = 0;
    int player1Depth = 1;
    int player2Depth = 1;
    DataManager dataManager;

/////////////////////////////  Testing Win Outcomes ////////////////////////////////////
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
/////////////////////////////  End of Testing Win Outcomes ////////////////////////////////////


/////////////////////////////  Testing Performance ////////////////////////////////////
//    std::priority_queue<TurnOutcome> boardProcessingSpeedsMinimax;
//    std::priority_queue<TurnOutcome> boardProcessingSpeedsAlphaBeta;
//    for(int playerDepth = 1; playerDepth < 5; playerDepth++) {
//        auto boardProcessingSpeedData = std::get<2>(playGame(saveTheGame, playerDepth, playerDepth, playTheGame, pruning));
//        while (!boardProcessingSpeedData.empty()) {
//            boardProcessingSpeedsMinimax.push(boardProcessingSpeedData.top());
//            boardProcessingSpeedData.pop();
//        }
//    }
//    std::cout << "Step | Depth | BPPS w/ minimax" << std::endl;
//    std::cout << "-------------------------------------------------------" << std::endl;
//    std::cout << boardProcessingSpeedsMinimax;
//    std::cout << std::endl;
//    pruning = 1;
//    for(int playerDepth = 1; playerDepth < 5; playerDepth++) {
//        auto boardProcessingSpeedData = std::get<2>(playGame(saveTheGame, playerDepth, playerDepth, playTheGame, pruning));
//        while (!boardProcessingSpeedData.empty()) {
//            boardProcessingSpeedsAlphaBeta.push(boardProcessingSpeedData.top());
//            boardProcessingSpeedData.pop();
//        }
//    }
//    std::cout << "Step | Depth | Bpps w/ alpha-beta" << std::endl;
//    std::cout << "-------------------------------------------------------" << std::endl;
//    std::cout << boardProcessingSpeedsAlphaBeta;
/////////////////////////////  End of Testing Performance ////////////////////////////////////


    std::cout << "Watch game or play game (0 = watch  1 = play)" << std::endl;
    std::cin >> playTheGame;

    std::cout << "Do you want to save this game (0 = no  1 = yes)" << std::endl;
    std::cin >> saveTheGame;

    std::cout << "Do you want pruning activated (0 = no  1 = yes)" << std::endl;
    std::cin >> pruning;

   if(playTheGame) {
       std::cout << "Select difficulty" << std::endl;
       std::cout << "0) Very Easy\n1) Easy\n2) Challenging\n3) Difficult\n4) Hard\n5) Very Hard" << std::endl;
       std::cin >> player2Depth;
   } else {
       std::cout << "Select bot 1 expert level" << std::endl;
       std::cout << "0) Dumb\n1) Novice\n2) Intermediate\n3) Skilled\n4) Expert\n5) Genius" << std::endl;
       std::cin >> player1Depth;
       std::cout << "Select bot 2 expert level" << std::endl;
       std::cout << "0) Dumb \n1) Novice\n2) Intermediate\n3) Skilled\n4) Expert\n5) Genius" << std::endl;
       std::cin >> player2Depth;
   }

    auto gameOutput = playGame(saveTheGame, player1Depth, player2Depth, playTheGame, pruning);

    if(!playTheGame) {
        if(std::get<0>(gameOutput)) {
            std::cout << ((std::get<1>(gameOutput) == o)? "o": "x") << " won" << std::endl;
        } else
                std::cout << "It's a tie" << std::endl;
    } else if(playTheGame) {
        if(std::get<0>(gameOutput)) {
            std::cout << "You won" << std::endl;
        } else
            std::cout << "x won" << std::endl;
    }

    if(saveTheGame) {
        std::cout << "Do you want a game recap (0 = no 1 = yes)" << std::endl;
        std::cin >> recapGame;
        if (recapGame)
            dataManager.loadGame();
    }

    return 0;
}
