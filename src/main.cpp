#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <utility>

enum class Player {
    NO_PLAYER = 0,
    PLAYER_A = 1,
    PLAYER_B = 2
};

enum class HexagonState {
    DEFAULT = 0,
    CLONE_OPTION = 1,
    JUMP_OPTION = 2,
    SELECTED = 3
};

enum class AdjacentHexagonsMode {
    TAKE_OVER_MODE = 0,
    CLONE_OPTIONS_VIEW_MODE = 1,
};

enum class GameState {
    Menu,
    Game
};

class Hexagon {
public:
    Hexagon(float x, float y, float size, sf::RenderWindow &window) : window(window) {
        shape.setPointCount(6);
        for (int i = 0; i < 6; ++i) {
            float angle = i * 2 * M_PI / 6;
            float xPos = x + size * std::cos(angle);
            float yPos = y + size * std::sin(angle);
            shape.setPoint(i, sf::Vector2f(xPos, yPos));
        }
        shape.setFillColor(sf::Color::White);
        shape.setOutlineThickness(3.0f);
        shape.setOutlineColor(sf::Color::Black);

        circle.setRadius(size * 0.6f);
        circle.setPosition(x - size * 0.6f, y - size * 0.6f);
        circle.setFillColor(sf::Color::Transparent);

        owner = Player::NO_PLAYER;
        currentState = HexagonState::DEFAULT;
    }

    void draw() {
        window.draw(shape);
        window.draw(circle);
    }

    bool containsCoordinates(float mouseX, float mouseY) const {
        return shape.getGlobalBounds().contains(mouseX, mouseY);
    }

    void setState(HexagonState state) {
        currentState = state;
        if (state == HexagonState::CLONE_OPTION) setFieldColor(sf::Color::Green);
        if (state == HexagonState::JUMP_OPTION) setFieldColor(sf::Color::Yellow);
        if (state == HexagonState::DEFAULT) setFieldColor(sf::Color::White);
    }

    HexagonState getState() {
        return currentState;
    }

    bool belongsToEnemy(Player currentPlayer) {
        return getOwner() != Player::NO_PLAYER &&
               getOwner() != currentPlayer;
    }

    void applyAdjacentHexagonsMode(AdjacentHexagonsMode mode, Player currentPlayer) {
        if (mode == AdjacentHexagonsMode::TAKE_OVER_MODE) {
            if (belongsToEnemy(currentPlayer))
                setOwner(currentPlayer);
        }
        if (mode == AdjacentHexagonsMode::CLONE_OPTIONS_VIEW_MODE) {
            if (getOwner() == Player::NO_PLAYER)
                setState(HexagonState::CLONE_OPTION);
        }
    }

    void setOwner(Player newOwner) {
        owner = newOwner;
        if (newOwner == Player::PLAYER_A) setCircleColor(sf::Color::Red);
        if (newOwner == Player::PLAYER_B) setCircleColor(sf::Color::Blue);
        if (newOwner == Player::NO_PLAYER) setCircleColor(sf::Color::Transparent);
    }

    Player getOwner() {
        return owner;
    }

private:
    float x{}, y{}, size{};
    Player owner;
    HexagonState currentState;
    sf::ConvexShape shape;
    sf::CircleShape circle;
    sf::RenderWindow &window;

    void setFieldColor(sf::Color color) {
        shape.setFillColor(color);
    }

    void setCircleColor(sf::Color color) {
        circle.setFillColor(color);
    }
};

class Counter {
public:
    Counter(sf::RenderWindow &window, Player player) : window(window) {
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

    void draw() {
        window.draw(text);
    }

    void updatePoints(int points) {
        std::string playerString = (owningPlayer == Player::PLAYER_A) ? "Player A" : "Player B";
        text.setString(playerString + "\n     " + std::to_string(points));
    }

private:
    sf::Font font;
    sf::Text text;
    Player owningPlayer;
    sf::RenderWindow &window;
};

class Board {
public:
    Board(int rows, int cols, float hexSize, sf::RenderWindow &window) : rows(rows), cols(cols), hexSize(hexSize),
                                                                         window(window), playerACounter(window, Player::PLAYER_A),
                                                                         playerBCounter(window, Player::PLAYER_B) {
        initializeHexagons();
        currentPlayer = Player::PLAYER_A;
    }

    void draw() {
        for (auto &col: hexagons) {
            for (auto &hexagon: col) {
                hexagon.draw();
            }
        }
        playerACounter.draw();
        playerBCounter.draw();
    }

    void onMouseClick(float mouseX, float mouseY) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < hexagons[i].size(); j++) {
                if (hexagons[i][j].containsCoordinates(mouseX, mouseY)) {

                    if (hexagons[i][j].getState() == HexagonState::DEFAULT || hexagons[i][j].getState() == HexagonState::SELECTED) {
                        resetStates();
                    }

                    if (hexagons[i][j].getOwner() == currentPlayer) {
                        hexagons[i][j].setState(HexagonState::SELECTED);

                        //GREEN FIELDS
                        setAdjacentHexagons(i, j, AdjacentHexagonsMode::CLONE_OPTIONS_VIEW_MODE);

                        //YELLOW FIELDS
                        setHexagonJumpOptions();

                        return;
                    }

                    if (hexagons[i][j].getState() == HexagonState::CLONE_OPTION) {
                        hexagons[i][j].setOwner(getSelectedHexagon().getOwner());
                        setAdjacentHexagons(i, j, AdjacentHexagonsMode::TAKE_OVER_MODE);

                        calculatePoints();
                        checkForWinner();
                        resetStates();
                        changePlayer();
                        return;
                    }

                    if (hexagons[i][j].getState() == HexagonState::JUMP_OPTION) {
                        Hexagon &selectedHexagon = getSelectedHexagon();

                        hexagons[i][j].setOwner(selectedHexagon.getOwner());
                        selectedHexagon.setOwner(Player::NO_PLAYER);
                        setAdjacentHexagons(i, j, AdjacentHexagonsMode::TAKE_OVER_MODE);

                        calculatePoints();
                        checkForWinner();
                        resetStates();
                        changePlayer();
                        return;
                    }
                }
            }
        }
    }

private:
    int rows, cols, playerAPoints, playerBPoints, emptyFields;
    float hexSize;
    sf::RenderWindow &window;
    std::vector<std::vector<Hexagon>> hexagons;
    Counter playerACounter, playerBCounter;
    Player currentPlayer;

    void initializeHexagons() {
        int hexagonInitialY = floor(rows / 2) * hexSize * sqrt(3);
        hexagons.resize(cols);

        for (int i = 0; i <= ceil(cols / 2); i++) {
            for (int j = 0; j < rows - i; j++) {
                float x = window.getSize().x / 2 - i * 1.5 * hexSize;
                float y = window.getSize().y / 2 + hexagonInitialY - i * hexSize * sqrt(3) / 2 - j * hexSize * sqrt(3);

                hexagons[ceil(cols / 2) - i].emplace_back(x, y, hexSize, window);
            }
        }
        for (int i = 0; i < floor(cols / 2); i++) {
            for (int j = 0; j < rows - 1 - i; j++) {
                float x = window.getSize().x / 2 + (i + 1) * 1.5 * hexSize;
                float y = window.getSize().y / 2 + hexagonInitialY - hexSize * sqrt(3) / 2 - i * hexSize * sqrt(3) / 2 -
                          j * hexSize * sqrt(3);

                hexagons[ceil(cols / 2) + 1 + i].emplace_back(x, y, hexSize, window);
            }
        }

        //ROWS AND COLUMNS CLEANUP + SETTING START POSITION FOR PLAYERS
        for (int i = 0; i < hexagons.size(); i++) {
            std::vector<Hexagon> reversedHexagons(hexagons[i].rbegin(), hexagons[i].rend());

            hexagons[i] = std::move(reversedHexagons);

            for (int j = 0; j < hexagons[i].size(); j++) {
                if (j == 0 && (i == 0 || i == hexagons.size() - 1)
                || i == hexagons.size() / 2 && j == hexagons[i].size() - 1)
                    hexagons[i][j].setOwner(Player::PLAYER_A);

                if (j == hexagons[i].size() - 1 && (i == 0 || i == hexagons.size() - 1)
                    || i == hexagons.size() / 2 && j == 0)
                    hexagons[i][j].setOwner(Player::PLAYER_B);
            }
        }
    }

    void changePlayer() {
        if (currentPlayer == Player::PLAYER_A) {
            currentPlayer = Player::PLAYER_B;
        } else {
            currentPlayer = Player::PLAYER_A;
        }
    }

    Hexagon &getHexagon(int column, int row) {
        for (int i = 0; i < hexagons.size(); i++) {
            for (int j = 0; j < hexagons[i].size(); j++) {
                if (i == column && j == row) {
                    return hexagons[i][j];
                }
            }
        }
    }

    Hexagon &getSelectedHexagon() {
        for (auto &col: hexagons) {
            for (auto &hexagon: col) {
                if (hexagon.getState() == HexagonState::SELECTED) {
                    return hexagon;
                }
            }
        }
    }

    std::pair<int, int> getSelectedHexagonBoardPosition() {
        for (int i = 0; i < hexagons.size(); i++) {
            for (int j = 0; j < hexagons[i].size(); j++) {
                if (hexagons[i][j].getState() == HexagonState::SELECTED) {
                    return std::make_pair(i, j);
                }
            }
        }
    }

    void calculatePoints() {
        playerAPoints = 0;
        playerBPoints = 0;
        emptyFields = 0;

        for (auto &col: hexagons) {
            for (auto &hexagon: col) {
                if (hexagon.getOwner() == Player::PLAYER_A) {
                    playerAPoints++;
                }
                if (hexagon.getOwner() == Player::PLAYER_B) {
                    playerBPoints++;
                }
                if (hexagon.getOwner() == Player::NO_PLAYER) {
                    emptyFields++;
                }
            }
        }

        playerACounter.updatePoints(playerAPoints);
        playerBCounter.updatePoints(playerBPoints);
    }

    void checkForWinner() {
        if (emptyFields == 0) {
            if (playerAPoints > playerBPoints) {
                std::cout << "PLAYER A WINS!";
                window.close();
            }
            if (playerBPoints > playerAPoints) {
                std::cout << "PLAYER B WINS!";
                window.close();
            }
            if (playerBPoints == playerAPoints) {
                std::cout << "DRAW!";
                window.close();
            }
        }
        if (playerBPoints == 0) {
            std::cout << "PLAYER A WINS!";
            window.close();
        }
        if (playerAPoints == 0) {
            std::cout << "PLAYER B WINS!";
            window.close();
        }
    }

    void resetStates() {
        for (auto &col: hexagons) {
            for (auto &hexagon: col) {
                hexagon.setState(HexagonState::DEFAULT);
            }
        }
    }

    void setAdjacentHexagons(int column, int row, AdjacentHexagonsMode mode) {
        //TOP HEXAGON
        if (row > 0) {
            auto &hexagon = getHexagon(column, row - 1);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        //RIGHT TOP HEXAGON
        if (column < (hexagons.size() / 2)) {
            auto &hexagon = getHexagon(column + 1, row);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        if (column >= (hexagons.size() / 2) && row > 0 && column < hexagons.size() - 1) {
            auto &hexagon = getHexagon(column + 1, row - 1);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        //RIGHT BOTTOM HEXAGON
        if (column < (hexagons.size() / 2)) {
            auto &hexagon = getHexagon(column + 1, row + 1);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        if (column >= (hexagons.size() / 2) && row < hexagons[column].size() - 1 && column < hexagons.size() - 1) {
            auto &hexagon = getHexagon(column + 1, row);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        //BOTTOM HEXAGON
        if (row < hexagons[column].size() - 1) {
            auto &hexagon = getHexagon(column, row + 1);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        //LEFT BOTTOM HEXAGON
        if (column > (hexagons.size() / 2)) {
            auto &hexagon = getHexagon(column - 1, row + 1);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        if (column <= (hexagons.size() / 2) && row < hexagons[column].size() - 1 && column > 0) {
            auto &hexagon = getHexagon(column - 1, row);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        //LEFT TOP HEXAGON
        if (column > (hexagons.size() / 2)) {
            auto &hexagon = getHexagon(column - 1, row);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
        if (column <= (hexagons.size() / 2) && row > 0 && column > 0) {
            auto &hexagon = getHexagon(column - 1, row - 1);
            hexagon.applyAdjacentHexagonsMode(mode, currentPlayer);
        }
    }

    void setHexagonJumpOptions() {
        std::pair<int, int> hexagonPosition = getSelectedHexagonBoardPosition();
        int column = hexagonPosition.first;
        int row = hexagonPosition.second;

        //TOP HEXAGON
        if (row > 1) {
            auto &hexagon = getHexagon(column, row - 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //TOP RIGHT FIRST HEXAGON
        if (column < (hexagons.size() / 2) && row > 0) {
            auto &hexagon = getHexagon(column + 1, row - 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row > 1 && column < hexagons.size() - 1) {
            auto &hexagon = getHexagon(column + 1, row - 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //TOP RIGHT SECOND HEXAGON
        if (column < (hexagons.size() / 2) - 1) {
            auto &hexagon = getHexagon(column + 2, row);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) - 1 && row > 0) {
            auto &hexagon = getHexagon(column + 2, row - 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row > 1 && column < hexagons.size() - 2) {
            auto &hexagon = getHexagon(column + 2, row - 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //RIGHT HEXAGON
        if (column < (hexagons.size() / 2) - 1) {
            auto &hexagon = getHexagon(column + 2, row + 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) - 1) {
            auto &hexagon = getHexagon(column + 2, row);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row > 0 && row < hexagons[column].size() - 1 &&
            column < hexagons.size() - 2) {
            auto &hexagon = getHexagon(column + 2, row - 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //BOTTOM RIGHT FIRST HEXAGON
        if (column < (hexagons.size() / 2) - 1) {
            auto &hexagon = getHexagon(column + 2, row + 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) - 1 && row < hexagons[column].size() - 1) {
            auto &hexagon = getHexagon(column + 2, row + 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column > (hexagons.size() / 2) - 1 && row < hexagons[column].size() - 2 && column < hexagons.size() - 2) {
            auto &hexagon = getHexagon(column + 2, row);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //BOTTOM RIGHT SECOND HEXAGON
        if (column < (hexagons.size() / 2) && row < hexagons[column].size() - 1) {
            auto &hexagon = getHexagon(column + 1, row + 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row < hexagons[column].size() - 2 && column < hexagons.size() - 1) {
            auto &hexagon = getHexagon(column + 1, row + 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //BOTTOM HEXAGON
        if (row < hexagons[column].size() - 2) {
            auto &hexagon = getHexagon(column, row + 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //BOTTOM LEFT FIRST HEXAGON
        if (column > (hexagons.size() / 2) && row < hexagons[column].size() - 1) {
            auto &hexagon = getHexagon(column - 1, row + 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row < hexagons[column].size() - 2 && column > 0) {
            auto &hexagon = getHexagon(column - 1, row + 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //BOTTOM LEFT SECOND HEXAGON
        if (column > (hexagons.size() / 2) + 1) {
            auto &hexagon = getHexagon(column - 2, row + 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) + 1 && row < hexagons[column].size() - 1) {
            auto &hexagon = getHexagon(column - 2, row + 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row < hexagons[column].size() - 2 && column > 1) {
            auto &hexagon = getHexagon(column - 2, row);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //LEFT HEXAGON
        if (column > (hexagons.size() / 2) + 1) {
            auto &hexagon = getHexagon(column - 2, row + 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) + 1) {
            auto &hexagon = getHexagon(column - 2, row);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row > 0 && row < hexagons[column].size() - 1 && column >= 2) {
            auto &hexagon = getHexagon(column - 2, row - 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //TOP LEFT FIRST HEXAGON
        if (column > (hexagons.size() / 2) + 1) {
            auto &hexagon = getHexagon(column - 2, row);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) + 1 && row > 0) {
            auto &hexagon = getHexagon(column - 2, row - 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row > 1 && column > 1) {
            auto &hexagon = getHexagon(column - 2, row - 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        //TOP LEFT SECOND HEXAGON
        if (column > (hexagons.size() / 2) && row > 0) {
            auto &hexagon = getHexagon(column - 1, row - 1);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row > 1 && column > 0) {
            auto &hexagon = getHexagon(column - 1, row - 2);
            if (hexagon.getOwner() == Player::NO_PLAYER)
                hexagon.setState(HexagonState::JUMP_OPTION);
        }
    }
};

class Game;

class Menu {
public:
    Menu(sf::RenderWindow &window, Game& game);

    void draw() {
        updateTextColors();

        window.draw(newGameText);
        window.draw(loadGameText);
        window.draw(exitText);
    }

    void onMouseClick(int mouseX, int mouseY);

private:
    sf::RenderWindow& window;
    sf::Text newGameText, loadGameText, exitText;
    sf::Font font;
    Game& game;

    void updateTextColors() {
        updateTextColor(newGameText);
        updateTextColor(loadGameText);
        updateTextColor(exitText);
    }

    void updateTextColor(sf::Text& text) {
        sf::FloatRect bounds = text.getGlobalBounds();
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        if (bounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
            text.setFillColor(sf::Color::Yellow);
        } else {
            text.setFillColor(sf::Color::White);
        }
    }
};

class Game {
public:
    Game(sf::RenderWindow& window) : window(window), gameState(GameState::Menu)  {}

    void run() {

        Menu mainMenu(window, *this);
        Board hexBoard(9, 9, 35, window);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();

                else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    float mouseX = static_cast<float>(event.mouseButton.x);
                    float mouseY = static_cast<float>(event.mouseButton.y);

                    if (gameState == GameState::Menu) {
                        mainMenu.onMouseClick(mouseX, mouseY);
                    }

                    if (gameState == GameState::Game) {
                        hexBoard.onMouseClick(mouseX, mouseY);
                    }
                }
            }

            window.clear();

            if (gameState == GameState::Menu) {
                mainMenu.draw();
            }
            if (gameState == GameState::Game) {
                hexBoard.draw();
            }

            window.display();
        }
    }

    void startGame() {
        gameState = GameState::Game;
    }

private:
    sf::RenderWindow& window;
    GameState gameState;
};

Menu::Menu(sf::RenderWindow &window, Game& game) : window(window), game(game) {
        if (!font.loadFromFile("../fonts/Silkscreen-Regular.ttf")) {
            throw std::runtime_error("Unable to load the font.");
        }

        newGameText.setFont(font);
        newGameText.setString("New game");
        newGameText.setCharacterSize(40);
        newGameText.setPosition((window.getSize().x - newGameText.getLocalBounds().width) / 2, 150);

        loadGameText.setFont(font);
        loadGameText.setString("Load game");
        loadGameText.setCharacterSize(40);
        loadGameText.setPosition((window.getSize().x - loadGameText.getLocalBounds().width) / 2, 200);

        exitText.setFont(font);
        exitText.setString("Exit");
        exitText.setCharacterSize(40);
        exitText.setPosition((window.getSize().x - exitText.getLocalBounds().width) / 2, 250);
}

void Menu::onMouseClick(int mouseX, int mouseY) {
    if (newGameText.getGlobalBounds().contains(mouseX, mouseY)) {
        game.startGame();
    } else if (loadGameText.getGlobalBounds().contains(mouseX, mouseY)) {
        std::cout << "Wczytaj grę\n";
    } else if (exitText.getGlobalBounds().contains(mouseX, mouseY)) {
        window.close();
    }
}

int main() {
    auto window = sf::RenderWindow{{1000, 600}, "Hexxagon"};

    Game game(window);
    game.run();

    return 0;
}