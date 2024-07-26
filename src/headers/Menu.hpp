#pragma once

#include <SFML/Graphics.hpp>

class Game;

class Menu {
public:
    Menu(sf::RenderWindow &window, Game &game);
    void draw();
    void onMouseClick(int mouseX, int mouseY);

private:
    sf::RenderWindow &window;
    sf::Text newGameText, loadGameText, exitText;
    sf::Font font;
    Game &game;

    void updateTextColors();
    void updateTextColor(sf::Text &text);
};