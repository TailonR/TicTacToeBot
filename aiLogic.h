//
// Created by renew on 3/9/2021.
//

#ifndef TICTACTOEBOT_AILOGIC_H
#define TICTACTOEBOT_AILOGIC_H
#include <vector>
#include <string>
#include <queue>
#include <iostream>
#include <chrono>

#include "board.h"
#include "player.h"
#include "node.h"
#include "turnOutcome.h"
#include "gameStates.h"

class Minimax {
public:
    using Place = std::pair<int, int>;
    Minimax(int, int);
    std::pair<Place, int> minimax(const Board & board, int depth, bool maximizingPlayer, Player players, int &boardCount);
private:
    void setUniformDistribution(int max);
    Player player1;
    Player player2;
    std::uniform_int_distribution<int> dist;
    std::mt19937 mt;
};

class AlphaBeta {
public:
    using Place = std::pair<int, int>;
    AlphaBeta(int, int);
    std::pair<Place, int> alphaBetaMinimax(const Board & board, int depth, int alpha, int beta, bool maximizingPlayer, Player player, int &boardCount);
    std::pair<Place, int> alphaBetaOffline(const Board & board, int alpha, int beta, bool maximizingPlayer, Player player, int &boardCount);

private:
    void setUniformDistribution(int max);
    Player player1;
    Player player2;
    std::uniform_int_distribution<int> dist;
    std::mt19937 mt;
};

class MCTS {
public:
    using Place = std::pair<int, int>;
    MCTS();
    Place search(const Board & board, int searchTime, std::vector<Player> players, int turn);
    std::shared_ptr<Player> simulation(Board currBoard, int turn, std::vector<Player> players, int & boardCount);
    void setUniformDistribution(int max);
private:
    int boardsProcessed = 0;
    std::uniform_int_distribution<int> dist;
    std::mt19937 mt;
};

int scorePosition(const Board & board, int playerType);
std::shared_ptr<Node> selection(const std::shared_ptr<Node>& root, Player player);
double ucbScore(int parentSimulations, int nodeSimulations, int nodeWinPercentage);
bool checkTerminalNode(const std::shared_ptr<Node>& node);
bool checkTerminalNode(const Board & board);
int evaluateBoard(const std::vector<std::string> & collection, int playerType);

#endif //TICTACTOEBOT_AILOGIC_H
