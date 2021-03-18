//
// Created by renew on 3/9/2021.
//

#ifndef TICTACTOEBOT_NODE_H
#define TICTACTOEBOT_NODE_H
#include <vector>
#include <memory>
#include <chrono>
#include <utility>
#include <algorithm>
#include <random>

#include "board.h"
#include "player.h"
struct PlayerStats {
    explicit PlayerStats(Player player);
    int playerWins;
    int playerType;
    int winFraction;
};

class Node {
public:
    using Place = std::pair<int, int>;
    Node(const Board & board, Place  move, const std::vector<Player> & thePlayers, int turn);
    std::vector<std::shared_ptr<Node>> getChildren() const;
    static void addChildren(const std::shared_ptr<Node>& node);
    std::shared_ptr<Node> getNewChild();
    const std::shared_ptr<Node>& getParent() const;
    Board getBoard() const;
    void update(Player player); // update the player who won
    bool canAddChildren() const;
    PlayerStats getStats(Player player) const;
    int getNumSimulations() const;
    Place getMove() const;
private:
    Board board;
    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children = {};
    std::vector<Place> unvisitedSpaces;
    Place currentMove;
    std::vector<Player> players;
    std::vector<PlayerStats> playerStats;
    int numSimulations;
    int playerTurn;
    std::uniform_int_distribution<int> dist;
    std::mt19937 mt;
};

bool operator== (const Node&, const Node&);
#endif //TICTACTOEBOT_NODE_H
