//
// Created by renew on 3/9/2021.
//

#include "dataManager.h"

// clear Data
// clears the game data
void DataManager::clearData() {
    std::ofstream open("gameData.txt");
    open.close();
}

// storeMove
// stores the gameplay
void DataManager::storeMove(const Board & theBoard){
    std::ofstream saver("gameData.txt", std::ios::app);

    for(int rowIndex = 0; rowIndex < theBoard.rowSize(); rowIndex++) {
        for(int colIndex = 0; colIndex < theBoard.colSize(); colIndex++) {
            const auto &cell = theBoard.getCell(rowIndex, colIndex);
            if (!cell.empty())
                saver << cell << " ";
            else
                saver << "_" << " ";
        }
        saver << std::endl;
    }
    saver.close();
}

// loadGame
// stores the gameplay
void DataManager::loadGame(){
    std::ifstream loader("gameData.txt");
    std::string line;
    int rowNumber = 0;
    while(std::getline(loader, line)) {
        if (line.substr(0,4) == "Turn")
            std::cout << "\n" << line << "\n" << std::endl;
        else
            std::cout << line << std::endl;

        rowNumber++;
        if(rowNumber == 5)
        {
            std::cout << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            rowNumber = 0;
        }
    }

}
