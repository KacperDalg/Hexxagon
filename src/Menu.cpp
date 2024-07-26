#include "headers/Menu.hpp"
#include "headers/Game.hpp"

Menu::Menu(sf::RenderWindow &window, Game &game) : window(window), game(game) {
    if (!font.loadFromFile("../fonts/Silkscreen-Regular.ttf")) {
        throw std::runtime_error("Unable to load the font.");
    }

    newGameText.setFont(font);
    newGameText.setString("New game");
    newGameText.setCharacterSize(40);
    newGameText.setPosition((window.getSize().x - newGameText.getLocalBounds().width) / 2, 200);

    loadGameText.setFont(font);
    loadGameText.setString("Load game");
    loadGameText.setCharacterSize(40);
    loadGameText.setPosition((window.getSize().x - loadGameText.getLocalBounds().width) / 2, 250);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(40);
    exitText.setPosition((window.getSize().x - exitText.getLocalBounds().width) / 2, 300);
}

void Menu::onMouseClick(int mouseX, int mouseY) {
    if (newGameText.getGlobalBounds().contains(mouseX, mouseY)) {
        game.startNewGame();
    } else if (loadGameText.getGlobalBounds().contains(mouseX, mouseY)) {
        game.openSavedGamesMenu();
    } else if (exitText.getGlobalBounds().contains(mouseX, mouseY)) {
        window.close();
    }
}

void Menu::draw() {
    updateTextColors();

    window.draw(newGameText);
    window.draw(loadGameText);
    window.draw(exitText);
}

void Menu::updateTextColors() {
    updateTextColor(newGameText);
    updateTextColor(loadGameText);
    updateTextColor(exitText);
}

void Menu::updateTextColor(sf::Text &text) {
    auto bounds = text.getGlobalBounds();
    auto mousePosition = sf::Mouse::getPosition(window);

    if (bounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
        text.setFillColor(sf::Color::Yellow);
    } else {
        text.setFillColor(sf::Color::White);
    }
}

