#pragma once

#include <SFML/Graphics.hpp>

class Game;

class SavedGamesMenu {
public:
    SavedGamesMenu(sf::RenderWindow &window, Game &game);

    void draw();

    void refresh();

    void onMouseClick(int mouseX, int mouseY);

private:
    sf::RenderWindow &window;
    std::vector<sf::Text> savedGames;
    sf::Text savedGamesText;
    sf::Font font;
    Game &game;

    void updateTextColors();

    void updateTextColor(sf::Text &text);
};