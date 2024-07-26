#pragma once

#include <SFML/Graphics.hpp>

class Game;

class PauseMenu {
public:
    PauseMenu(sf::RenderWindow &window, Game &game);
    void draw();
    void onMouseClick(int mouseX, int mouseY);

private:
    sf::RenderWindow &window;
    sf::Text backToGameText, saveAndExitText, exitText;
    sf::RectangleShape background;
    sf::Font font;
    Game &game;

    void updateTextColors();
    void updateTextColor(sf::Text &text);
};