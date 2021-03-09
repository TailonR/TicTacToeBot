//
// Created by renew on 3/9/2021.
//

#ifndef TICTACTOEBOT_DATAMANAGER_H
#define TICTACTOEBOT_DATAMANAGER_H
#include <thread>
#include <iostream>
#include "player.h"
#include "board.h"
#include "aiLogic.h"

class DataManager{
public:
    void loadGame();
    void clearData();
    void storeMove(const Board & theBoard);
};
#endif //TICTACTOEBOT_DATAMANAGER_H
