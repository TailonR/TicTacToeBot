// main.cpp
// Author: Tailon Russell
// The purpose of this file is to contain the main
//      program to start a tic tac toe game
#include <queue>
#include "player.h"
#include "turnOutcome.h"
#include "aiLogic.h"
#include "dataManager.h"

// playGame
// This is the function for the gameplay
std::tuple<bool, int, std::priority_queue<TurnOutcome>> playGame(int save, int player1Depth, int player2Depth, int playGame, bool pruning, int searchTime) {
    Board board(5,5); // 5x5 board
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> randRow(0, board.rowSize());
    std::uniform_int_distribution<int> randCol(0, board.colSize());
    enum type {o = 0, x = 1, none};
    std::vector<Player> players = {Player(o), Player(x)};
//    Minimax minimax (players[0].getPlayerType(), players[1].getPlayerType());
    AlphaBeta alphaBeta (players[0].getPlayerType(), players[1].getPlayerType());
    MCTS mcts;
    DataManager dataManager;

    std::tuple<bool, int, std::priority_queue<TurnOutcome>> gameWon;
    std::priority_queue<TurnOutcome> turnOutcomes;
    bool tie = false;

    bool saveGame = save;
    bool repeat;
    int turn = o;
    int turnCount = 0;
    int userRow = -1;
    int userCol = -1;
    int boardCount = 0;
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
                    players[0].makeMove(board, userRow, userCol);
                else
                    repeat = true;
            } else {
                if(player1Depth == 0) {
                    int row = randRow(mt);
                    int col = randCol(mt);
                    if(board.validLocation(row, col))
                        players[0].makeMove(board, row, col);
                    else
                        repeat = true;

               } else {
                    clock_t start = clock();
                    auto player1Move = (pruning)? std::get<0>(alphaBeta.alphaBetaMinimax(board, player1Depth, -1000000000, 1000000000, true, players[0], boardCount)):
                                       mcts.search(board, searchTime, players, turn);
                    clock_t end = clock();
                    auto timeSpent = (end-start)/CLOCKS_PER_SEC;
                    players[0].makeMove(board, std::get<0>(player1Move), std::get<1>(player1Move));
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
                int row = randRow(mt);
                int col = randCol(mt);
                if(board.validLocation(row, col)) {
                    players[1].makeMove(board, row, col);
                }
                else
                    repeat = true;
            } else {
                boardCount = 0;
                clock_t start = clock();
                auto player2Move = (!pruning)? std::get<0>(alphaBeta.alphaBetaMinimax(board, player2Depth, -1000000000, 1000000000, true, players[1], boardCount)):
                                    mcts.search(board, searchTime, players, turn);

                clock_t end = clock();
                auto timeSpent = (end-start)/CLOCKS_PER_SEC;
                players[1].makeMove(board, std::get<0>(player2Move), std::get<1>(player2Move));
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

    auto gameOutput = playGame(saveTheGame, player1Depth, player2Depth, playTheGame, pruning, 60);

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


/////////////////////// Part A //////////////////////////////
//    std::vector<Player> players = { Player(o), Player(x)};
//    MCTS mcts(players[0].getPlayerType(), players[1].getPlayerType());
//    AlphaBeta alphaBeta(players[0].getPlayerType(), players[1].getPlayerType());
//    Minimax minimax(players[0].getPlayerType(), players[1].getPlayerType());
//    Board board(5, 5);
//
//    bool tie = false;
//    int turn;
//    int turnCount = 0;
//    int boardCount = 0;
//
////    std::cout << board.board;
////    std::cout << std::endl;
//
////    while (!std::get<0>(board.gameWinningMove()) && !tie) {
////        std::vector<std::vector<int>> boardScores (board.rowSize(), std::vector<int>(board.colSize()));
////        if(turn == o) {
////            turnCount++;
//////            auto player1Move = std::get<0>(minimax.minimax(board, 3, true, players[0], boardCount));
//////            players[0].makeMove(board, std::get<0>(player1Move), std::get<1>(player1Move));
////
////            auto start = clock();
//////            auto player1Move = mcts.search(board, 60, players, 1);
////            auto player1Move = std::get<0>(alphaBeta.alphaBetaMinimax(board, 4, -1000000000, 1000000000, true, players[1], boardCount));
////            auto end = clock();
////            int duration = (end - start)/CLOCKS_PER_SEC;
////            players[0].makeMove(board, std::get<0>(player1Move), std::get<1>(player1Move));
////            boardCount = mcts.getBoardsProcessed();
////            int bpps = boardCount/(duration>0?duration: 1);
////            std::cout << turnCount << "-" << boardCount << "-" << bpps << std::endl;
////        }
////        else {
////            turnCount++;
////            auto start = clock();
////            auto player2Move = mcts.search(board, 60, players, 1);
////            auto end = clock();
////            int duration = (end - start)/CLOCKS_PER_SEC;
////            players[1].makeMove(board, std::get<0>(player2Move), std::get<1>(player2Move));
////            boardCount = mcts.getBoardsProcessed();
////            int bpps = boardCount/duration;
////            std::cout << turnCount << "-" << boardCount << "-" << bpps << std::endl;
////        }
////        turn = !turn;
////
//////        std::cout << board.board;
//////        std::cout << std::endl;
////        if(board.getValidLocations().empty())
////            tie = true;
////    }
////    std::cout << (std::get<0>(board.gameWinningMove())? (std::get<1>(board.gameWinningMove())? "x": "o"): "No one") << " won" << std::endl;
///////////////////// End Part A ////////////////////////////


/////////////////////// Part B //////////////////////////////
//    std::vector<Player> players = {Player(o), Player(x)};
//    Board onlineBoard(5, 5);
////    onlineBoard.board[0][0] = "o";
////    onlineBoard.board[0][1] = "o";
////    onlineBoard.board[0][2] = "o";
////    onlineBoard.board[0][3] = "x";
//    onlineBoard.board[0][4] = "x";
//// ------------------------------
//    onlineBoard.board[1][0] = "x";
//    onlineBoard.board[1][1] = "x";
//    onlineBoard.board[1][2] = "o";
//    onlineBoard.board[1][3] = "x";
////    onlineBoard.board[1][4] = "x";
//// ------------------------------
////    onlineBoard.board[2][0] = "x";
//    onlineBoard.board[2][1] = "x";
//    onlineBoard.board[2][2] = "o";
//    onlineBoard.board[2][3] = "o";
////    onlineBoard.board[2][4] = "x";
//// ------------------------------
////    onlineBoard.board[3][0] = "x";
//    onlineBoard.board[3][1] = "o";
////    onlineBoard.board[3][2] = "x";
////    onlineBoard.board[3][3] = "o";
////    onlineBoard.board[3][4] = "x";
//// ------------------------------
////    onlineBoard.board[4][0] = "x";
////    onlineBoard.board[4][1] = "x";
//    onlineBoard.board[4][2] = "o";
//    onlineBoard.board[4][3] = "o";
////    onlineBoard.board[4][4] = "o";
//    Board offlineBoard = onlineBoard;
////    Board offlineBoard(5,5);
//
//    AlphaBeta alphaBeta(players[0].getPlayerType(), players[1].getPlayerType());
//    MCTS mcts(players[0].getPlayerType(), players[1].getPlayerType());
//
//    std::cout << "The current onlineBoard: " << std::endl;
//    std::cout << onlineBoard.board;
//    std::cout << std::endl;
//
//    int boardCount = 0;
//
//    auto start = std::chrono::high_resolution_clock::now();
////    auto playerMove = std::get<0>(alphaBeta.alphaBetaMinimax(onlineBoard, 4, -1000000000, 1000000000, true, players[0], boardCount));
//    auto elapsed = std::chrono::high_resolution_clock::now() - start;
//    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
////    std::cout << "The best move found using regular alpha beta: " << std::get<0>(playerMove) << ", " << std::get<1>(playerMove) << "   time elapsed: " << microseconds << std::endl;
//
//    start = std::chrono::high_resolution_clock::now();
//    auto playerMoveOffline = std::get<0>(alphaBeta.alphaBetaOffline(offlineBoard, -1000000000, 1000000000, true, players[1], boardCount));
//    elapsed = std::chrono::high_resolution_clock::now() - start;
//    microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
//    std::cout << "The best move found using offline alpha beta: " << std::get<0>(playerMoveOffline) << ", " << std::get<1>(playerMoveOffline) << "   time elapsed: " << microseconds << std::endl;
//
//    int searchTime = 5;
//    std::pair<int,int> playerMoveMCTS;
//    bool firstAttempt = true;
//    while(true) {
//        if(!firstAttempt)
//            std::cout << "Trying again" << std::endl;
//        playerMoveMCTS = mcts.search(offlineBoard, searchTime, players, 1);
//        firstAttempt = false;
//        if (std::get<0>(playerMoveOffline) == std::get<0>(playerMoveMCTS))
//            if (std::get<1>(playerMoveOffline) == std::get<1>(playerMoveMCTS))
//                break;
//
//        searchTime += 5;
//    }
//    std::cout << "The best move found using mcts: " << std::get<0>(playerMoveMCTS) << ", " << std::get<1>(playerMoveMCTS) << "   search time: " << searchTime << std::endl;

//    bool tie = false;
//    int turn;
//    while (!std::get<0>(onlineBoard.gameWinningMove()) && !tie) {
//        std::vector<std::vector<int>> boardScores (onlineBoard.rowSize(), std::vector<int>(onlineBoard.colSize()));
//        if(turn == o) {
////            auto player1Move = mcts.search(onlineBoard, 15, players, 0);
//            auto player1Move = std::get<0>(alphaBeta.alphaBetaMinimax(onlineBoard, 4, -1000000000, 1000000000, true, players[0], boardCount));
//            players[0].makeMove(onlineBoard, std::get<0>(player1Move), std::get<1>(player1Move));
//        }
//        else {
//            auto player2Move = std::get<0>(alphaBeta.alphaBetaMinimax(onlineBoard, 4, -1000000000, 1000000000, true, players[1], boardCount));;
//            players[1].makeMove(onlineBoard, std::get<0>(player2Move), std::get<1>(player2Move));
//        }
//        turn = !turn;
//
//        std::cout << onlineBoard.board;
//        std::cout << std::endl;
//        if(onlineBoard.getValidLocations().empty())
//            tie = true;
//    }
///////////////////// End Part B ////////////////////////////

    return 0;
}
