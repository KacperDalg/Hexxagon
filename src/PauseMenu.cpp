#include "headers/PauseMenu.hpp"
#include "headers/Game.hpp"

PauseMenu::PauseMenu(sf::RenderWindow &window, Game &game) : window(window), game(game) {
    if (!font.loadFromFile("../fonts/Silkscreen-Regular.ttf")) {
        throw std::runtime_error("Unable to load the font.");
    }

    backToGameText.setFont(font);
    backToGameText.setString("Back to game");
    backToGameText.setCharacterSize(25);
    backToGameText.setPosition((window.getSize().x - backToGameText.getLocalBounds().width) / 2, 230);

    saveAndExitText.setFont(font);
    saveAndExitText.setString("Save and exit");
    saveAndExitText.setCharacterSize(25);
    saveAndExitText.setPosition((window.getSize().x - saveAndExitText.getLocalBounds().width) / 2, 280);

    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(25);
    exitText.setPosition((window.getSize().x - exitText.getLocalBounds().width) / 2, 330);

    float squareSizeX = 400.0f;
    float squareSizeY = 300.0f;

    float squarePosX = (window.getSize().x - squareSizeX) / 2;
    float squarePosY = (window.getSize().y - squareSizeY) / 2;

    background.setSize(sf::Vector2f(squareSizeX, squareSizeY));
    background.setPosition(squarePosX, squarePosY);
    background.setFillColor(sf::Color::Black);

    background.setOutlineThickness(5);
    background.setOutlineColor(sf::Color::Yellow);
}

void PauseMenu::draw() {
    updateTextColors();

    window.draw(background);
    window.draw(backToGameText);
    window.draw(saveAndExitText);
    window.draw(exitText);
}

void PauseMenu::onMouseClick(int mouseX, int mouseY) {
    if (backToGameText.getGlobalBounds().contains(mouseX, mouseY)) {
        game.switchToGame();
    } else if (saveAndExitText.getGlobalBounds().contains(mouseX, mouseY)) {
        game.saveGame();
        game.openMainMenu();
    } else if (exitText.getGlobalBounds().contains(mouseX, mouseY)) {
        game.openMainMenu();
    }
}

void PauseMenu::updateTextColors() {
    updateTextColor(backToGameText);
    updateTextColor(saveAndExitText);
    updateTextColor(exitText);
}

void PauseMenu::updateTextColor(sf::Text &text) {
    auto mousePosition = sf::Mouse::getPosition(window);

    if (text.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
        text.setFillColor(sf::Color::Yellow);
    } else {
        text.setFillColor(sf::Color::White);
    }
}

