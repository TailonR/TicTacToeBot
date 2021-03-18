//
// Created by renew on 3/9/2021.
//

#include "board.h"
Board::Board(): board(5, std::vector<std::string>(5, "")) {}

Board::Board(int row, int col): board(row, std::vector<std::string>(col, "")) {}

int Board::rowSize() const {
    return board.size();
}

int Board::colSize() const {
    return board[0].size();
}

std::string Board::getCell(int row, int col) const {
    return board[row][col];
}

int Board::capacity() const {
    return board.capacity();
}

// getValidLocations
// returns a vector of all locations that are valid (as determined by the function validLocation)
std::vector<std::pair<int, int>> Board::getValidLocations() const{
    std::vector<std::pair<int, int>> validLocations;
    for(int rIndex = 0; rIndex < board.size(); rIndex++)
        for(int cIndex = 0; cIndex < board[0].size(); cIndex++)
            if (validLocation(rIndex, cIndex))
                validLocations.emplace_back(rIndex, cIndex);
    return validLocations;
}

// const gameWinningMove
// Checks if there is a win on the board
// What's left:
//      Think about return a pair <bool, int> the int is the player type that won
std::pair<bool, int> Board::gameWinningMove() const{
    enum type {o = 0, x = 1, none};
    // Check Horizontal win
    for(const auto & rowIndex : board) {
        for(int colIndex = 0; colIndex < board[0].size()-3; colIndex++) {
            if(!rowIndex[colIndex].empty() && rowIndex[colIndex+1] == rowIndex[colIndex] &&
               rowIndex[colIndex+2] == rowIndex[colIndex] && rowIndex[colIndex+3] == rowIndex[colIndex])
                return std::make_pair(true, (rowIndex[colIndex] == "o"? o: x));
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

// validLocation
// Checks if the place specified by the given row and column is a valid space
//      meaning if it's on the board and if there is no other marker that's already there
bool Board::validLocation(int row, int col) const {
    if(row < board.size() && col < board[0].size() && board[row][col].empty())
        return true;
    return false;
}

void Board::setCell(int row, int col, const std::string& marker) {
    board[row][col] = marker;
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


