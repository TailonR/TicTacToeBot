//
// Created by renew on 3/9/2021.
//

#ifndef TICTACTOEBOT_BOARD_H
#define TICTACTOEBOT_BOARD_H
#include <vector>
#include <string>
#include <fstream>
#include <utility>

class Board {
public:
    Board(int row, int col);
    void createBoard(int row, int col);
    std::vector<std::vector<std::string>> board;
    bool validLocation(int row, int col);
    bool validLocation(int row, int col) const;
    int rowSize();
    int rowSize() const;
    int colSize();
    int colSize() const;
    std::string& getCell(int row, int col);
    const std::string& getCell(int row, int col) const;
    std::vector<std::string>& getRow(int row);
    const std::vector<std::string>& getRow(int row) const;
    int capacity();
    const int capacity() const;
    std::vector<std::pair<int, int>> getValidLocations();
    const std::vector<std::pair<int, int>> getValidLocations() const;
    std::pair<bool, int> gameWinningMove();
    const std::pair<bool, int> gameWinningMove() const;
};
std::ostream& operator<< (std::ostream& os,  const std::vector<std::vector<std::string>> & board);


#endif //TICTACTOEBOT_BOARD_H
