cmake_minimum_required(VERSION 3.17)
project(TicTacToeBot)

set(CMAKE_CXX_STANDARD 17)

add_executable(TicTacToeBot main.cpp doctest.h player.cpp player.h board.cpp board.h turnOutcome.cpp turnOutcome.h aiLogic.cpp aiLogic.h dataManager.cpp dataManager.h node.cpp node.h gameStates.cpp gameStates.h)
