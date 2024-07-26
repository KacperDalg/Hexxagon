#pragma once

#include "Enums.hpp"
#include <SFML/Graphics.hpp>

class Counter {
public:
    Counter(sf::RenderWindow &window, Player player);

    void draw();

    void updatePoints(int points);

private:
    sf::Font font;
    sf::Text text;
    Player owningPlayer;
    sf::RenderWindow &window;
};