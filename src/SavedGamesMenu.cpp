#include "headers/SavedGamesMenu.hpp"
#include "headers/Game.hpp"

SavedGamesMenu::SavedGamesMenu(sf::RenderWindow &window, Game &game) : window(window), game(game) {
    if (!font.loadFromFile("../fonts/Silkscreen-Regular.ttf")) {
        throw std::runtime_error("Unable to load the font.");
    }

    savedGamesText.setFont(font);
    savedGamesText.setFillColor(sf::Color::Yellow);
    savedGamesText.setString("Saved games");
    savedGamesText.setCharacterSize(60);
    savedGamesText.setPosition((window.getSize().x - savedGamesText.getLocalBounds().width) / 2, 10);
}

void SavedGamesMenu::draw() {
    updateTextColors();

    for (auto &text: savedGames) {
        window.draw(text);
    }
    window.draw(savedGamesText);
}

void SavedGamesMenu::refresh() {
    savedGames.clear();

    auto folderPath = "../saved";

    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directory(folderPath);
    }

    int positionY = 70;
    std::vector<std::filesystem::directory_entry> entries;
    for (auto &file: std::filesystem::directory_iterator(folderPath)) {
        entries.emplace_back(file);
    }

    std::reverse(entries.begin(), entries.end());

    auto i = int();
    for (auto &entry: entries) {
        sf::Text savedGame;
        savedGame.setFont(font);
        savedGame.setString(entries[i].path().filename().string());
        savedGame.setCharacterSize(30);
        savedGame.setPosition((window.getSize().x - savedGame.getLocalBounds().width) / 2, positionY += 50);
        savedGames.emplace_back(savedGame);
        i++;
        if (i == 8) break;
    }
}

void SavedGamesMenu::onMouseClick(int mouseX, int mouseY) {
    for (auto &savedGame: savedGames) {
        if (savedGame.getGlobalBounds().contains(mouseX, mouseY)) {
            game.loadGame(savedGame.getString().toAnsiString());
        }
    }
}

void SavedGamesMenu::updateTextColors() {
    for (auto &text: savedGames) {
        updateTextColor(text);
    }
}

void SavedGamesMenu::updateTextColor(sf::Text &text) {
    auto bounds = text.getGlobalBounds();
    auto mousePosition = sf::Mouse::getPosition(window);

    if (bounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
        text.setFillColor(sf::Color::Yellow);
    } else {
        text.setFillColor(sf::Color::White);
    }
}