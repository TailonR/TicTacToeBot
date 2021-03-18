//
// Created by renew on 3/9/2021.
//

#include "aiLogic.h"
#include <algorithm>
#include <utility>
#include <cmath>
//////////////////////////////////////////////////////////////// MiniMax ///////////////////////////////////////////////////////////////////////////////////
Minimax::Minimax(int player1Type, int player2Type): player1(player1Type), player2(player2Type), mt(std::random_device{}()) {}

void Minimax::setUniformDistribution(int max) {
    std::uniform_int_distribution<int> range(0, max);
    dist = range;
}

// minimax
// determines the behavior of the ai in placing markers
std::pair<Minimax::Place, int> Minimax::minimax(const Board &board, int depth, bool maximizingPlayer,
                                                    Player player, int &boardCount) {
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
            auto score = scorePosition(board, (player.getPlayerType()==0? o: x));
            boardCount++;
            return std::make_pair(std::make_pair(-1,-1), score);
        }
    }

    if (maximizingPlayer) {
        int score = -1000000000;
        int newScore;
        setUniformDistribution(validLocations.size()-1);
        auto bestLocation = validLocations[dist(mt)];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            if (player.getPlayerType() == 0) {
                player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(minimax(tempBoard, depth-1, false, player2, boardCount));
            } else {
                player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(minimax(tempBoard, depth-1, false, player1, boardCount));
            }
            if (newScore > score) {
                score = newScore;
                bestLocation = location;
            }
        }
        return std::make_pair(bestLocation, score);

    } else { //Minimizing player
        int score = 1000000000;
        int newScore;
        setUniformDistribution(validLocations.size()-1);
        auto bestLocation = validLocations[dist(mt)];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            if (player.getPlayerType() == 0) {
                player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(minimax(tempBoard, depth-1, true, player1, boardCount));
            } else {
                player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(minimax(tempBoard, depth-1, true, player2, boardCount));
            }
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
AlphaBeta::AlphaBeta(int player1Type, int player2Type): player1(player1Type), player2(player2Type), mt(std::random_device{}()) {}

void AlphaBeta::setUniformDistribution(int max) {
    std::uniform_int_distribution<int> range(0, max);
    dist = range;
}

// alphaBetaMinimax
// determines the behavior of the ai in placing markers
std::pair<AlphaBeta::Place, int> AlphaBeta::alphaBetaMinimax(const Board & board, int depth, int alpha, int beta, bool maximizingPlayer, Player player, int &boardCount) {
    auto validLocations = board.getValidLocations();
    enum type {o = 0, x = 1, none};
    bool isTerminal = checkTerminalNode(board);
    if (depth == 0 || isTerminal) {
        if (isTerminal) {
            boardCount++;
            if (std::get<0>(board.gameWinningMove())) {
                if(player.getPlayerType() == std::get<1>(board.gameWinningMove()))
                    return std::make_pair(std::make_pair(-1,-1), 1000000000);
                else if (player.getPlayerType() != std::get<1>(board.gameWinningMove()))
                    return std::make_pair(std::make_pair(-1,-1), -1000000000);
            } else {
                return std::make_pair(std::make_pair(-1,-1), 0);
            }
        }
        else {
            auto score = scorePosition(board, (player.getPlayerType()==0? o: x));
            boardCount++;
            return std::make_pair(std::make_pair(-1,-1), score);
        }
    }


    if (maximizingPlayer) {
        int score = -1000000000;
        int newScore;
        setUniformDistribution(validLocations.size() - 1);
        auto bestLocation = validLocations[dist(mt)];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            if (player.getPlayerType() == 0) {
                player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaMinimax(tempBoard, depth-1, alpha, beta, false, player2, boardCount));
            } else {
                player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaMinimax(tempBoard, depth-1, alpha, beta, false, player1, boardCount));
            }
            if (newScore > score) {
                score = newScore;
                bestLocation = location;
            }
            alpha = std::max(alpha, score);
            if(alpha >= beta) {
                if(tempBoard.getValidLocations().size() < 21) {
//                    std::cout << "-------------------- size = " << tempBoard.getValidLocations().size() << "-----------------------" << std::endl;
//                    std::cout << "The board prior to bad move: " << std::endl;
//                    std::cout << board.board << std::endl;
//                    std::cout << "The board after to bad move: " << std::endl;
//                    std::cout << tempBoard.board << std::endl;
//                    std::cout << std::endl;
//                    std::cout << "-------------------- size = " << tempBoard.getValidLocations().size() << "-----------------------" << std::endl;
                }
                break;
            }
        }
        return std::make_pair(bestLocation, score);

    } else { //Minimizing player
        int score = 1000000000;
        int newScore;
        setUniformDistribution(validLocations.size() - 1);
        auto bestLocation = validLocations[dist(mt)];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            if (player.getPlayerType() == 0) {
                player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaMinimax(tempBoard, depth-1, alpha, beta, true, player1, boardCount));
            } else {
                player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaMinimax(tempBoard, depth-1, alpha, beta, true, player2, boardCount));
            }
            if(newScore < score) {
                score = newScore;
                bestLocation = location;
            }
            beta = std::min(beta, score);
            if (alpha >= beta) {
                if(tempBoard.getValidLocations().size() < 8) {
//                    int size = tempBoard.getValidLocations().size();
//                    std::cout << "-------------------- size = " << size << "-----------------------" << std::endl;
//                    std::cout << "The board prior to bad move: " << std::endl;
//                    std::cout << board.board << std::endl;
//                    std::cout << "The board after to bad move: " << std::endl;
//                    std::cout << tempBoard.board << std::endl;
//                    std::cout << std::endl;
//                    std::cout << "-------------------- size = " << size << "-----------------------" << std::endl;
                }
                break;
            }
        }
        return std::make_pair(bestLocation, score);
    }
}

// alphaBetaMinimax (offline)
// determines the behavior of the ai in placing markers while not in game
std::pair<AlphaBeta::Place, int> AlphaBeta::alphaBetaOffline(const Board &board, int alpha, int beta,
                                                                bool maximizingPlayer, Player player,
                                                                int &boardCount) {
    auto validLocations = board.getValidLocations();
    enum type {o = 0, x = 1, none};
    bool isTerminal = checkTerminalNode(board);
    if (isTerminal) {
        boardCount++;
        if (std::get<0>(board.gameWinningMove())) {
            if(player.getPlayerType() == std::get<1>(board.gameWinningMove()))
                return std::make_pair(std::make_pair(-1,-1), 1000000000);
            else if (player.getPlayerType() != std::get<1>(board.gameWinningMove()))
                return std::make_pair(std::make_pair(-1,-1), -1000000000);
        } else {
            return std::make_pair(std::make_pair(-1,-1), 0);
        }
    }


    if (maximizingPlayer) {
        int score = -1000000000;
        int newScore;
        setUniformDistribution(validLocations.size() - 1);
        auto bestLocation = validLocations[dist(mt)];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            if (player.getPlayerType() == 0) {
                player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaOffline(tempBoard, alpha, beta, false, player2, boardCount));
            } else {
                player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaOffline(tempBoard, alpha, beta, false, player1, boardCount));
            }
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
        int newScore;
        setUniformDistribution(validLocations.size() - 1);
        auto bestLocation = validLocations[dist(mt)];
        for(auto & location: validLocations) {
            auto tempBoard = board;
            if (player.getPlayerType() == 0) {
                player2.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaOffline(tempBoard, alpha, beta, true, player1, boardCount));
            } else {
                player1.makeMove(tempBoard, std::get<0>(location), std::get<1>(location));
                newScore = std::get<1>(alphaBetaOffline(tempBoard, alpha, beta, true, player2, boardCount));
            }
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

///////////////////////////////////////////////////////////// Monte Carlo Tree Search /////////////////////////////////////////////////////////////////

MCTS::MCTS(): mt(std::random_device{}()) {}

MCTS::Place MCTS::search(const Board & board, int searchTime, std::vector<Player> players, int turn) {
    boardsProcessed = 0;

    Node root(board, std::make_pair(-1,-1), players, turn);
    std::shared_ptr<Node> node(&root);

    while(!node->canAddChildren() && !checkTerminalNode(node)) {
        node = selection(node, players[turn]);
    }

    if (node->canAddChildren()) {
        Node::addChildren(node);
        node->getNewChild();
    }

    std::shared_ptr<Player> winner;
    clock_t end =  0;

    clock_t start = clock();
    while((end - start)/CLOCKS_PER_SEC < searchTime) {// The iterative loop -- util time is up
        winner = simulation(node->getBoard(), turn, players, boardsProcessed);
        end = clock();
    }

    while (node != nullptr && winner != nullptr) {
        node->update(*winner);
        node = node->getParent();
    }

    Place bestMove;
    int bestWinPercentage = -1;
    for (auto & child: root.getChildren()) {
        int childWinPercentage = child->getStats(players[!turn]).winFraction;
        if(childWinPercentage > bestWinPercentage) {
            bestWinPercentage = childWinPercentage;
            bestMove = child->getMove();
        }
    }
    return bestMove;
}

void MCTS::setUniformDistribution(int max) {
    std::uniform_int_distribution<int> range(0, max);
    dist = range;
}

// This simulates finishing the game if the proposed node was chosen
// Eventually gonna need player type
std::shared_ptr<Player> MCTS::simulation(Board currBoard, int turn, std::vector<Player> players, int & boardCount) {
    auto validLocations = currBoard.getValidLocations();
    setUniformDistribution(validLocations.size() - 1);
    auto nextMove = validLocations[dist(mt)];
    players[turn].makeMove(currBoard, std::get<0>(nextMove), std::get<1>(nextMove));
    while (!std::get<0>(currBoard.gameWinningMove()) && !validLocations.empty()) {
        boardCount++;
        validLocations = currBoard.getValidLocations();
        if(validLocations.empty())
            break;
        setUniformDistribution(validLocations.size() - 1);
        nextMove = validLocations[dist(mt)];
        players[!turn].makeMove(currBoard, std::get<0>(nextMove), std::get<1>(nextMove));
    }
    return std::make_shared<Player> (std::get<1>(currBoard.gameWinningMove()));
}

/////////////////////////////////////////////////////////// End Monte Carlo Tree Search ///////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////// Global Functions ///////////////////////////////////////////////////////////////
// evaluateBoard
// The evaluation function
// It evaluates the given places on a board based on the player type (o or x)
//      and returns a value based on how many player markers are already in the given places
//      and how many opponent markers there are in the given places
int evaluateBoard(const std::vector<std::string> &collection, int playerType) {
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
        score -= 950;
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
bool checkTerminalNode(const Board & board) {
    return std::get<0>(board.gameWinningMove()) || board.getValidLocations().empty();
}

bool checkTerminalNode(const std::shared_ptr<Node>& node) {
    return std::get<0>(node->getBoard().gameWinningMove()) || node->getBoard().getValidLocations().empty();
}

double ucbScore(int parentSimulations, int nodeSimulations, int nodeWinPercentage) {
    double exploration = sqrt(nodeSimulations > 0? log(parentSimulations)/nodeSimulations: log(parentSimulations));
    return nodeWinPercentage + sqrt(2) * exploration;
}

std::shared_ptr<Node> selection(const std::shared_ptr<Node>& node, Player player) {
    // This selects the node to take
    std::cout << "Do you see me?" << std::endl;
    int totalSimulations = 0;
    for (const auto& child: node->getChildren())
        totalSimulations += child->getNumSimulations();

    double bestVal = -1.0;
    std::shared_ptr<Node> bestChild;
    double currVal;
    for(auto & child: node->getChildren()) {
        currVal = ucbScore(totalSimulations,
                           node->getNumSimulations(),
                           node->getStats(player).winFraction);

        if(currVal > bestVal) {
            bestVal = currVal;
            bestChild = node;
        }
    }

    return bestChild;
}

// scorePosition
// places value of the horizontal, vertical, and diagonal rows on the board based on the player type (o or x)
int scorePosition(const Board & board, int playerType) {
    {
        int numInRowToWin = 4;
        int score = 0;

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
////////////////////////////////////////////////////////////// End of Global Functions ///////////////////////////////////////////////////////////////////////
