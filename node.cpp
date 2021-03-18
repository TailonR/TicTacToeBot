//
// Created by renew on 3/9/2021.
//

#include "node.h"

#include <utility>

PlayerStats::PlayerStats(Player player): playerType(player.getPlayerType()), playerWins(0), winFraction(0) {}

Node::Node(const Board & currentBoard, Place  move, const std::vector<Player> & thePlayers, int turn):  board(currentBoard), parent(nullptr),
                                                                                                        currentMove(std::move(move)), players(thePlayers), playerTurn(turn),
                                                                                                        playerStats({ PlayerStats(thePlayers[0]), PlayerStats(thePlayers[1])}),
                                                                                                        unvisitedSpaces(currentBoard.getValidLocations()),
                                                                                                        numSimulations(0), mt(std::random_device{}()), dist(0, unvisitedSpaces.size()) {}

std::vector<std::shared_ptr<Node>> Node::getChildren() const {
    return children;
}

const std::shared_ptr<Node>& Node::getParent() const {
    return parent;
}

Board Node::getBoard() const {
    return board;
}

std::shared_ptr<Node> Node::getNewChild() {
    return children[children.size()-1];
}

void Node::addChildren(const std::shared_ptr<Node>& node) {
    auto newBoard = node->board;
    auto newMove = node->unvisitedSpaces[node->dist(node->mt)];
    node->unvisitedSpaces.erase(std::find(node->unvisitedSpaces.begin(), node->unvisitedSpaces.end(), newMove));

    if(node->playerTurn)
        node->players[1].makeMove(newBoard, std::get<0>(newMove), std::get<1>(newMove));
    else
        node->players[0].makeMove(newBoard, std::get<0>(newMove), std::get<1>(newMove));

    auto newNode = std::make_shared<Node>(newBoard, newMove, node->players, !(node->playerTurn));
    newNode->parent = node;
    node->children.emplace_back(newNode);
}

void Node::update(Player player) {
    auto playerToBeUpdated = std::find_if(playerStats.begin(), playerStats.end(), [player](PlayerStats possiblePlayer){
        return possiblePlayer.playerType == player.getPlayerType();
    });
    playerToBeUpdated->playerWins++;
    numSimulations++;
    playerToBeUpdated->winFraction = playerToBeUpdated->playerWins/numSimulations;
}

bool Node::canAddChildren() const {
    return !unvisitedSpaces.empty();
}

int Node::getNumSimulations() const {
    return numSimulations;
}

PlayerStats Node::getStats(Player player) const {
    auto playerToBeUpdated = std::find_if(playerStats.begin(), playerStats.end(), [player](PlayerStats possiblePlayer){
        return possiblePlayer.playerType == player.getPlayerType();
    });

    return *playerToBeUpdated;
}

Node::Place Node::getMove() const {
    return currentMove;
}

bool operator==(const Node& lhs, const Node& rhs) {
    return (lhs.getParent() == rhs.getParent() && lhs.getChildren() == rhs.getChildren());
}
