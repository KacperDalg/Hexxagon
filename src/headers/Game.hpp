#pragma once

#include "Board.hpp"
#include "PauseMenu.hpp"
#include "Menu.hpp"
#include "SavedGamesMenu.hpp"

class Game {
public:
    Game(sf::RenderWindow &window);

    void run();

    void switchToGame();

    void startNewGame();

    void openMainMenu();

    void openSavedGamesMenu();

    void saveGame();

    void loadGame(std::string const& fileName);

private:
    sf::RenderWindow &window;
    GameState gameState;
    Board hexBoard;
    Menu mainMenu;
    PauseMenu pauseMenu;
    SavedGamesMenu savedGamesMenu;
};