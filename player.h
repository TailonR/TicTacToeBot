//
// Created by renew on 3/9/2021.
//

#ifndef TICTACTOEBOT_PLAYER_H
#define TICTACTOEBOT_PLAYER_H
#include <vector>
#include <string>
#include "board.h"

class Player {
public:
    using Place = std::pair<int, int>;
    explicit Player(int type);
    void makeMove(Board & theBoard, int row, int col);
    int getPlayerType() const;
private:
    int playerType;
};
#endif //TICTACTOEBOT_PLAYER_H
