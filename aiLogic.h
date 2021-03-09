//
// Created by renew on 3/9/2021.
//

#ifndef TICTACTOEBOT_AILOGIC_H
#define TICTACTOEBOT_AILOGIC_H
#include <vector>
#include <string>
#include "board.h"
#include "player.h"


class Minimax {
public:
    Minimax(int, int);
    int scorePosition(const Board & board, int playerType);
    int evaluateBoard(const std::vector<std::string> & collection, int playerType);
    bool checkTerminalNode(const Board & board);
    std::pair<std::pair<int, int>, int> minimax(const Board & board, int depth, bool maximizingPlayer, int playerType, int &boardCount);

private:
    Player player1;
    Player player2;
};

class AlphaBeta {
public:
    AlphaBeta(int, int);
    int scorePosition(const Board & board, int playerType);
    int evaluateBoard(const std::vector<std::string> & collection, int playerType);
    bool checkTerminalNode(const Board & board);
    std::pair<std::pair<int, int>, int> alphaBetaMinimax(const Board & board, int depth, int alpha, int beta, bool maximizingPlayer, int playerType, int &boardCount);

private:
    Player player1;
    Player player2;
};

class MCTS {

};
#endif //TICTACTOEBOT_AILOGIC_H
