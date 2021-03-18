//
// Created by renew on 3/9/2021.
//

#include "player.h"
Player::Player(int type): playerType(type) {}

int Player::getPlayerType() const {
    return playerType;
}

// makeMove
// inserts the player symbol on the board in the given location
void Player::makeMove(Board & theBoard, int row, int col){
    if (playerType) {
        theBoard.setCell(row,col, "x");
    } else if(playerType == 0) {
        theBoard.setCell(row, col, "o");
    }
}
