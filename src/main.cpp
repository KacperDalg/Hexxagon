#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

enum class Owner {
    NO_OWNER = 0,
    PLAYER_A = 1,
    PLAYER_B = 2
};

enum class State {
    DEFAULT = 0,
    CLONE_OPTION = 1,
    JUMP_OPTION = 2,
    SELECTED = 3
};

class Hexagon {
public:
    Hexagon(float x, float y, float size, sf::RenderWindow& window) : window(window) {
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

            circle.setRadius(size * 0.6f); // Promień koła 60% wielkości heksagonu
            circle.setPosition(x - size * 0.6f, y - size * 0.6f);
            circle.setFillColor(sf::Color::Transparent);

            owner = Owner::NO_OWNER;
            currentState = State::DEFAULT;
    }

    void draw() {
        window.draw(shape);
        window.draw(circle);
    }

    bool containsCoordinates(float mouseX, float mouseY) const {
        return shape.getGlobalBounds().contains(mouseX, mouseY);
    }

    void setState(State state) {
        currentState = state;
        if (state == State::CLONE_OPTION) setColor(sf::Color::Green);
        if (state == State::JUMP_OPTION) setColor(sf::Color::Yellow);
        if (state == State::DEFAULT) setColor(sf::Color::White);
    }

    State getState() {
        return currentState;
    }

    void setOwner(Owner newOwner) {
        owner = newOwner;
        if (newOwner == Owner::PLAYER_A) setCircleColor(sf::Color::Red);
        if (newOwner == Owner::PLAYER_B) setCircleColor(sf::Color::Blue);
        if (newOwner == Owner::NO_OWNER) setCircleColor(sf::Color::Transparent);
    }

    Owner getOwner() {
        return owner;
    }

private:
    float x{}, y{}, size{};
    Owner owner;
    State currentState;
    sf::ConvexShape shape;
    sf::CircleShape circle;
    sf::RenderWindow& window;

    void setColor(sf::Color color) {
        shape.setFillColor(color);
    }

    void setCircleColor(sf::Color color) {
        circle.setFillColor(color);
    }
};

class Board {
public:
    Board(int rows, int cols, float hexSize, sf::RenderWindow& window) : rows(rows), cols(cols), hexSize(hexSize), window(window) {
        initializeHexagons();
    }

    void draw() {
        for (auto& col : hexagons) {
            for (auto& hexagon : col) {
                hexagon.draw();
            }
        }
    }

    void onMouseClick(float mouseX, float mouseY) {
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < hexagons[i].size(); j++) {
                if (hexagons[i][j].containsCoordinates(mouseX, mouseY)
                    && (hexagons[i][j].getState() == State::DEFAULT || hexagons[i][j].getState() == State::SELECTED)) {
                    resetStates();
                }

                if (hexagons[i][j].containsCoordinates(mouseX, mouseY) && hexagons[i][j].getOwner() != Owner::NO_OWNER) {
                    hexagons[i][j].setState(State::SELECTED);

                    //ZIELONE POLA
                    setHexagonCloneOptions(i, j);

                    //ZOLTE POLA
                    setHexagonJumpOptions(i, j);

                    return;
                }

                if (hexagons[i][j].containsCoordinates(mouseX, mouseY) && hexagons[i][j].getState() == State::CLONE_OPTION) {
                    hexagons[i][j].setOwner(getSelectedHexagon().getOwner());
                    resetStates();

                    return;
                }

                if (hexagons[i][j].containsCoordinates(mouseX, mouseY) && hexagons[i][j].getState() == State::JUMP_OPTION) {
                    Hexagon& selectedHexagon = getSelectedHexagon();

                    hexagons[i][j].setOwner(selectedHexagon.getOwner());
                    selectedHexagon.setOwner(Owner::NO_OWNER);
                    resetStates();

                    return;
                }
            }
        }
    }

private:
    int rows, cols;
    float hexSize;
    sf::RenderWindow& window;
    std::vector<std::vector<Hexagon>> hexagons;

    void initializeHexagons() {
        int hexagonInitialY = floor(rows/2) * hexSize * sqrt(3);
        hexagons.resize(cols);

        for (int i = 0; i <= ceil(cols/2); i++) {
            for (int j = 0; j < rows - i; j++) {
                float x = window.getSize().x / 2 - i * 1.5 * hexSize;
                float y = window.getSize().y / 2 + hexagonInitialY - i * hexSize * sqrt(3) / 2 - j * hexSize * sqrt(3);

                hexagons[ceil(cols/2) - i].emplace_back(x, y, hexSize, window);
            }
        }
        for (int i = 0; i < floor(cols/2); i++) {
            for (int j = 0; j < rows - 1 - i; j++){
                float x = window.getSize().x / 2 + (i + 1) * 1.5 * hexSize;
                float y = window.getSize().y / 2 + hexagonInitialY - hexSize * sqrt(3) / 2 - i * hexSize * sqrt(3) / 2 - j * hexSize * sqrt(3);

                hexagons[ceil(cols/2) + 1 + i].emplace_back(x, y, hexSize, window);
            }
        }

        //ROWS AND COLUMNS CLEANUP + SETTING START POSITION FOR PLAYERS
        for(int i = 0; i < hexagons.size(); i++) {
            std::vector<Hexagon> reversedHexagons(hexagons[i].rbegin(), hexagons[i].rend());

            hexagons[i] = std::move(reversedHexagons);

            for(int j = 0; j < hexagons[i].size(); j++) {
                if(i == 0 && (j == 0 || j == hexagons[i].size() - 1)) hexagons[i][j].setOwner(Owner::PLAYER_A);
                if(i == hexagons.size() - 1 && (j == 0 || j == hexagons[i].size() - 1)) hexagons[i][j].setOwner(Owner::PLAYER_B);
            }
        }
    }

    Hexagon& getSelectedHexagon() {
        for (auto& col : hexagons) {
            for (auto& hexagon : col) {
                if (hexagon.getState() == State::SELECTED) {
                    return hexagon;
                }
            }
        }
    }

    void setHexagonState(int column, int row, State state) {
        if (hexagons[column][row].getOwner() == Owner::NO_OWNER)
            hexagons[column][row].setState(state);
    }

    void resetStates() {
        for (auto& col : hexagons) {
            for (auto& hexagon : col) {
                hexagon.setState(State::DEFAULT);
            }
        }
    }

    void setHexagonCloneOptions(int column, int row) {
        //DOLNY HEXAGON
        if (row < hexagons[column].size() - 1) {
            setHexagonState(column, row + 1, State::CLONE_OPTION);
        }
        //GORNY HEXAGON
        if (row > 0) {
            setHexagonState(column, row - 1, State::CLONE_OPTION);
        }
        //PRAWY GORNY HEXAGON
        if (column < (hexagons.size() / 2)) {
            setHexagonState(column + 1, row, State::CLONE_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row > 0 && column < hexagons.size() - 1) {
            setHexagonState(column + 1, row - 1, State::CLONE_OPTION);
        }
        //PRAWY DOLNY HEXAGON
        if (column < (hexagons.size() / 2)) {
            setHexagonState(column + 1, row + 1, State::CLONE_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row < hexagons[column].size() - 1 && column < hexagons.size() - 1) {
            setHexagonState(column + 1, row, State::CLONE_OPTION);
        }
        //LEWY GORNY HEXAGON
        if (column > (hexagons.size() / 2)) {
            setHexagonState(column - 1, row, State::CLONE_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row > 0 && column > 0) {
            setHexagonState(column - 1, row - 1, State::CLONE_OPTION);
        }
        //LEWY DOLNY HEXAGON
        if (column > (hexagons.size() / 2)) {
            setHexagonState(column - 1, row + 1, State::CLONE_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row < hexagons[column].size() - 1 && column > 0) {
            setHexagonState(column - 1, row, State::CLONE_OPTION);
        }
    }

    void setHexagonJumpOptions(int column, int row) {
        //GORNY HEXAGON
        if (row > 1) {
            setHexagonState(column, row - 2, State::JUMP_OPTION);
        }
        //PRAWY GÓRNY PIERWSZY HEXAGON
        if (column < (hexagons.size() / 2) && row > 0) {
            setHexagonState(column + 1, row - 1, State::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row > 1 && column < hexagons.size() - 1) {
            setHexagonState(column + 1, row - 2, State::JUMP_OPTION);
        }
        //PRAWY GÓRNY DRUGI HEXAGON
        if (column < (hexagons.size() / 2) - 1) {
            setHexagonState(column + 2, row, State::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) - 1 && row > 0) {
            setHexagonState(column + 2, row - 1, State::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row > 1 && column < hexagons.size() - 2) {
            setHexagonState(column + 2, row - 2, State::JUMP_OPTION);
        }
        //PRAWY HEXAGON
        if (column < (hexagons.size() / 2) - 1) {
            setHexagonState(column + 2, row + 1, State::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) - 1) {
            setHexagonState(column + 2, row, State::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row > 0 && row < hexagons[column].size() - 1 && column < hexagons.size() - 2) {
            setHexagonState(column + 2, row - 1, State::JUMP_OPTION);
        }
        //PRAWY DOLNY PIERWSZY HEXAGON
        if (column < (hexagons.size() / 2) - 1) {
            setHexagonState(column + 2, row + 2, State::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) - 1 && row < hexagons[column].size() - 1) {
            setHexagonState(column + 2, row + 1, State::JUMP_OPTION);
        }
        if (column > (hexagons.size() / 2) - 1 && row < hexagons[column].size() - 2 && column < hexagons.size() - 2) {
            setHexagonState(column + 2, row, State::JUMP_OPTION);
        }
        //PRAWY DOLNY DRUGI HEXAGON
        if (column < (hexagons.size() / 2) && row < hexagons[column].size() - 1) {
            setHexagonState(column + 1, row + 2, State::JUMP_OPTION);
        }
        if (column >= (hexagons.size() / 2) && row < hexagons[column].size() - 2 && column < hexagons.size() - 1) {
            setHexagonState(column + 1, row + 1, State::JUMP_OPTION);
        }
        //DOLNY HEXAGON
        if (row < hexagons[column].size() - 2) {
            setHexagonState(column, row + 2, State::JUMP_OPTION);
        }
        //LEWY DOLNY PIERWSZY HEXAGON
        if (column > (hexagons.size() / 2) && row < hexagons[column].size() - 1) {
            setHexagonState(column - 1, row + 2, State::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row < hexagons[column].size() - 2 && column > 0) {
            setHexagonState(column - 1, row + 1, State::JUMP_OPTION);
        }
        //LEWY DOLNY DRUGI HEXAGON
        if (column > (hexagons.size() / 2) + 1) {
            setHexagonState(column - 2, row + 2, State::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) + 1 && row < hexagons[column].size() - 1) {
            setHexagonState(column - 2, row + 1, State::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row < hexagons[column].size() - 2 && column > 1) {
            setHexagonState(column - 2, row, State::JUMP_OPTION);
        }
        //LEWY HEXAGON
        if (column > (hexagons.size() / 2) + 1) {
            setHexagonState(column - 2, row + 1, State::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) + 1) {
            setHexagonState(column - 2, row, State::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row > 0 && row < hexagons[column].size() - 1 && column >= 2) {
            setHexagonState(column - 2, row - 1, State::JUMP_OPTION);
        }
        //LEWY GORNY PIERWSZY HEXAGON
        if (column > (hexagons.size() / 2) + 1) {
            setHexagonState(column - 2, row, State::JUMP_OPTION);
        }
        if (column == (hexagons.size() / 2) + 1 && row > 0) {
            setHexagonState(column - 2, row - 1, State::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row > 1 && column > 1) {
            setHexagonState(column - 2, row - 2, State::JUMP_OPTION);
        }
        //LEWY GORNY DRUGI HEXAGON
        if (column > (hexagons.size() / 2) && row > 0) {
            setHexagonState(column - 1, row - 1, State::JUMP_OPTION);
        }
        if (column <= (hexagons.size() / 2) && row > 1 && column > 0) {
            setHexagonState(column - 1, row - 2, State::JUMP_OPTION);
        }
    }
};

int main()
{
    auto window = sf::RenderWindow{ { 1000, 600 }, "Hexxagon" };
    window.setFramerateLimit(144);

    Board hexBoard(9, 9, 35, window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                float mouseX = static_cast<float>(event.mouseButton.x);
                float mouseY = static_cast<float>(event.mouseButton.y);
                hexBoard.onMouseClick(mouseX, mouseY);
            }
        }

        window.clear();

        hexBoard.draw();

        window.display();
    }
}