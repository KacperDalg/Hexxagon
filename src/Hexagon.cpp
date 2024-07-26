#include "headers/Hexagon.hpp"
#include <cmath>

Hexagon::Hexagon(float x, float y, float size, sf::RenderWindow &window) : window(window) {
    //https://stackoverflow.com/questions/37236439/creating-a-single-hexagon-in-c-sharp-using-drawpolygon
    //https://www.sfml-dev.org/tutorials/2.0/graphics-shape.php
    shape.setPointCount(6);
    for (int i = 0; i < 6; i++) {
        float xPos = x + size * std::cos(i * 60 * M_PI / 180);
        float yPos = y + size * std::sin(i * 60 * M_PI / 180);
        shape.setPoint(i, sf::Vector2f(xPos, yPos));
    }
    shape.setFillColor(sf::Color::White);
    shape.setOutlineThickness(3.0f);
    shape.setOutlineColor(sf::Color::Black);

    circle.setRadius(size * 0.6f);
    circle.setPosition(x - size * 0.6f, y - size * 0.6f);
    circle.setFillColor(sf::Color::Transparent);

    setOwner(Player::NO_PLAYER);
    setState(HexagonState::DEFAULT);
}

void Hexagon::draw() {
    window.draw(shape);
    window.draw(circle);
}

bool Hexagon::containsCoordinates(float mouseX, float mouseY) const {
    return shape.getGlobalBounds().contains(mouseX, mouseY);
}

void Hexagon::setState(HexagonState state) {
    currentState = state;
    if (state == HexagonState::CLONE_OPTION) setFieldColor(sf::Color::Green);
    if (state == HexagonState::JUMP_OPTION) setFieldColor(sf::Color::Yellow);
    if (state == HexagonState::DEFAULT) setFieldColor(sf::Color::White);
}

HexagonState Hexagon::getState() {
    return currentState;
}

bool Hexagon::belongsToEnemy(Player currentPlayer) {
    return getOwner() != Player::NO_PLAYER &&
           getOwner() != currentPlayer;
}

void Hexagon::applyAdjacentHexagonsMode(AdjacentHexagonsMode mode, Player currentPlayer) {
    if (mode == AdjacentHexagonsMode::TAKE_OVER_MODE) {
        if (belongsToEnemy(currentPlayer))
            setOwner(currentPlayer);
    }
    if (mode == AdjacentHexagonsMode::CLONE_OPTIONS_VIEW_MODE) {
        if (getOwner() == Player::NO_PLAYER)
            setState(HexagonState::CLONE_OPTION);
    }
}

void Hexagon::setOwner(Player newOwner) {
    owner = newOwner;
    if (newOwner == Player::PLAYER_A) setCircleColor(sf::Color::Red);
    if (newOwner == Player::PLAYER_B) setCircleColor(sf::Color::Blue);
    if (newOwner == Player::NO_PLAYER) setCircleColor(sf::Color::Transparent);
}

Player Hexagon::getOwner() {
    return owner;
}

void Hexagon::setFieldColor(sf::Color color) {
    shape.setFillColor(color);
}

void Hexagon::setCircleColor(sf::Color color) {
    circle.setFillColor(color);
}

//https://stackoverflow.com/questions/11635172/how-to-implement-swap
void swap(Hexagon &first, Hexagon &second) {
    using std::swap;
    swap(first.x, second.x);
    swap(first.y, second.y);
    swap(first.size, second.size);
    swap(first.owner, second.owner);
    swap(first.currentState, second.currentState);
    swap(first.shape, second.shape);
    swap(first.circle, second.circle);
}