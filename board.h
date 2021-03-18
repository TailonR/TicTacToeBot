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
    Board();
    Board(int row, int col);
    std::vector<std::vector<std::string>> board;
    bool validLocation(int row, int col) const;
    int rowSize() const;
    int colSize() const;
    std::string getCell(int row, int col) const;
    void setCell(int row, int col, const std::string& marker);
    int capacity() const;
    std::vector<std::pair<int, int>> getValidLocations() const;
    std::pair<bool, int> gameWinningMove() const;
};
std::ostream& operator<< (std::ostream& os,  const std::vector<std::vector<std::string>> & board);


#endif //TICTACTOEBOT_BOARD_H
