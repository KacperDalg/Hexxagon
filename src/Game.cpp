#include "headers/Game.hpp"

Game::Game(sf::RenderWindow &window) : window(window), gameState(GameState::Menu), hexBoard(9, 9, 35, window),
                                       savedGamesMenu(window, *this), pauseMenu(window, *this),
                                       mainMenu(window, *this) {}

void Game::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                auto mouseX = static_cast<float>(event.mouseButton.x);
                auto mouseY = static_cast<float>(event.mouseButton.y);

                if (gameState == GameState::Menu) {
                    mainMenu.onMouseClick(mouseX, mouseY);
                } else if (gameState == GameState::SavedGamesMenu) {
                    savedGamesMenu.onMouseClick(mouseX, mouseY);
                } else if (gameState == GameState::Game) {
                    hexBoard.onMouseClick(mouseX, mouseY);
                } else if (gameState == GameState::Paused) {
                    pauseMenu.onMouseClick(mouseX, mouseY);
                }
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape && gameState == GameState::Game) {
                    gameState = GameState::Paused;
                } else if (event.key.code == sf::Keyboard::Escape && gameState == GameState::Paused) {
                    gameState = GameState::Game;
                }
            }
        }

        window.clear();

        if (gameState == GameState::Menu) {
            mainMenu.draw();
        }
        if (gameState == GameState::SavedGamesMenu) {
            savedGamesMenu.draw();
        }
        if (gameState == GameState::Game) {
            hexBoard.draw();
        }
        if (gameState == GameState::Paused) {
            hexBoard.draw();
            pauseMenu.draw();
        }

        window.display();
    }
}

void Game::switchToGame() {
    gameState = GameState::Game;
}

void Game::startNewGame() {
    hexBoard.start();
    gameState = GameState::Game;
}

void Game::openMainMenu() {
    gameState = GameState::Menu;
}

void Game::openSavedGamesMenu() {
    savedGamesMenu.refresh();
    gameState = GameState::SavedGamesMenu;
}

void Game::saveGame() {
    hexBoard.save();
}

void Game::loadGame(std::string const& fileName) {
    hexBoard.load(fileName);
    gameState = GameState::Game;
}

