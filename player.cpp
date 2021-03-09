//
// Created by renew on 3/9/2021.
//

#include "player.h"
Player::Player(int type): playerType(type) {}

int Player::getPlayerType() {
    return playerType;
}
// makeMove
// inserts the player symbol on the board in the given location
void Player::makeMove(Board & theBoard, int row, int col, int playerType){
    if (playerType) {
        theBoard.getCell(row,col) = "x";
    } else if(playerType == 0) {
        theBoard.getCell(row, col) = "o";
    }
}
