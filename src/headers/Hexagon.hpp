#pragma once

#include <SFML/Graphics.hpp>
#include "Enums.hpp"

class Hexagon {
public:
    Hexagon(float x, float y, float size, sf::RenderWindow &window);

    void draw();

    bool containsCoordinates(float mouseX, float mouseY) const;

    void setState(HexagonState state);

    HexagonState getState();

    bool belongsToEnemy(Player currentPlayer);

    void applyAdjacentHexagonsMode(AdjacentHexagonsMode mode, Player currentPlayer);

    void setOwner(Player newOwner);

    Player getOwner();

    //https://stackoverflow.com/questions/11635172/how-to-implement-swap
    friend void swap(Hexagon &first, Hexagon &second);

private:
    float x, y, size;
    Player owner;
    HexagonState currentState;
    sf::ConvexShape shape;
    sf::CircleShape circle;
    sf::RenderWindow &window;

    void setFieldColor(sf::Color color);

    void setCircleColor(sf::Color color);
};