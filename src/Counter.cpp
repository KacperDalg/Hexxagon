#pragma once

#include "headers/Counter.hpp"

Counter::Counter(sf::RenderWindow &window, Player player) : window(window) {
    if (!font.loadFromFile("../fonts/Silkscreen-Regular.ttf")) {
        throw std::runtime_error("Unable to load the font.");
    }

    text.setFont(font);
    text.setCharacterSize(30);

    owningPlayer = player;
    if (player == Player::PLAYER_A) {
        text.setPosition(20, 20);
        text.setFillColor(sf::Color::Red);
    }
    if (player == Player::PLAYER_B) {
        text.setPosition(window.getSize().x - 190, 20);
        text.setFillColor(sf::Color::Blue);
    }

    updatePoints(3);
}

void Counter::draw() {
    window.draw(text);
}

void Counter::updatePoints(int points) {
    std::string playerString = (owningPlayer == Player::PLAYER_A) ? "Player A" : "Player B";
    text.setString(playerString + "\n     " + std::to_string(points));
}