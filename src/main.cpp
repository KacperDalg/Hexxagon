#include <SFML/Graphics.hpp>
#include <cmath>

enum class Owner {
    NO_OWNER = 0,
    PLAYER_A = 1,
    PLAYER_B = 2
};

class Hexagon {
public:
    Hexagon(float x, float y, float size, Owner owner) {
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
    }

    void draw(sf::RenderWindow& renderWindow) {
        renderWindow.draw(shape);
        renderWindow.draw(circle);
    }

    bool containsCoordinates(float mouseX, float mouseY) const {
        return shape.getGlobalBounds().contains(mouseX, mouseY);
    }

    void setHexagonColor(sf::Color color) {
        shape.setFillColor(color);
    }

    void setOwner(Owner newOwner) {
        owner = newOwner;
        if (newOwner == Owner::PLAYER_A) circle.setFillColor(sf::Color::Red);
        if (newOwner == Owner::PLAYER_B) circle.setFillColor(sf::Color::Blue);
        if (newOwner == Owner::NO_OWNER) circle.setFillColor(sf::Color::Transparent);
    }

    Owner getOwner() {
        return owner;
    }

private:
    float x, y, size;
    Owner owner;
    sf::ConvexShape shape;
    sf::CircleShape circle;
};

class Board {
public:
    Board(int rows, int cols, float hexSize, sf::RenderWindow& window) : rows(rows), cols(cols), hexSize(hexSize), window(window) {
        initializeHexagons();
    }

    void draw() {
        for (auto& col : hexagons) {
            for (auto& hexagon : col) {
                hexagon.draw(window);
            }
        }
    }

    void clearColors() {
        for (auto& col : hexagons) {
            for (auto& hexagon : col) {
                hexagon.setHexagonColor(sf::Color::White);
                hexagon.draw(window);
            }
        }
    }

    void onMouseClick(float mouseX, float mouseY) {
        clearColors();
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < hexagons[i].size(); j++) {
                if (hexagons[i][j].containsCoordinates(mouseX, mouseY) && hexagons[i][j].getOwner() != Owner::NO_OWNER) {
                    //DOLNY HEXAGON
                    if (j < hexagons[i].size() - 1) {
                        if (hexagons[i][j + 1].getOwner() == Owner::NO_OWNER) hexagons[i][j + 1].setHexagonColor(sf::Color::Green);
                    }
                    //GORNY HEXAGON
                    if (j > 0) {
                        if (hexagons[i][j - 1].getOwner() == Owner::NO_OWNER) hexagons[i][j - 1].setHexagonColor(sf::Color::Green);
                    }
                    //PRAWY GORNY HEXAGON
                    if (i < (hexagons.size() / 2)) {
                        if (hexagons[i + 1][j].getOwner() == Owner::NO_OWNER) hexagons[i + 1][j].setHexagonColor(sf::Color::Green);
                    }
                    if (i >= (hexagons.size() / 2) && j > 0 && i < hexagons.size() - 1) {
                        if (hexagons[i + 1][j - 1].getOwner() == Owner::NO_OWNER) hexagons[i + 1][j - 1].setHexagonColor(sf::Color::Green);
                    }
                    //PRAWY DOLNY HEXAGON
                    if (i < (hexagons.size() / 2)) {
                        if (hexagons[i + 1][j + 1].getOwner() == Owner::NO_OWNER) hexagons[i + 1][j + 1].setHexagonColor(sf::Color::Green);
                    }
                    if (i >= (hexagons.size() / 2) && j < hexagons[i].size() - 1 && i < hexagons.size() - 1) {
                        if (hexagons[i + 1][j].getOwner() == Owner::NO_OWNER) hexagons[i + 1][j].setHexagonColor(sf::Color::Green);
                    }
                    //LEWY GORNY HEXAGON
                    if (i > (hexagons.size() / 2)) {
                        if (hexagons[i - 1][j].getOwner() == Owner::NO_OWNER) hexagons[i - 1][j].setHexagonColor(sf::Color::Green);
                    }
                    if (i <= (hexagons.size() / 2) && j > 0 && i > 0) {
                        if (hexagons[i - 1][j - 1].getOwner() == Owner::NO_OWNER) hexagons[i - 1][j - 1].setHexagonColor(sf::Color::Green);
                    }
                    //LEWY DOLNY HEXAGON
                    if (i > (hexagons.size() / 2)) {
                        if (hexagons[i - 1][j + 1].getOwner() == Owner::NO_OWNER) hexagons[i - 1][j + 1].setHexagonColor(sf::Color::Green);
                    }
                    if (i <= (hexagons.size() / 2) && j < hexagons[i].size() - 1 && i > 0) {
                        if (hexagons[i - 1][j].getOwner() == Owner::NO_OWNER) hexagons[i - 1][j].setHexagonColor(sf::Color::Green);
                    }

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

                hexagons[ceil(cols/2) - i].emplace_back(x, y, hexSize, Owner::NO_OWNER);
            }
        }
        for (int i = 0; i < floor(cols/2); i++) {
            for (int j = 0; j < rows - 1 - i; j++){
                float x = window.getSize().x / 2 + (i + 1) * 1.5 * hexSize;
                float y = window.getSize().y / 2 + hexagonInitialY - hexSize * sqrt(3) / 2 - i * hexSize * sqrt(3) / 2 - j * hexSize * sqrt(3);

                hexagons[ceil(cols/2) + 1 + i].emplace_back(x, y, hexSize, Owner::NO_OWNER);
            }
        }

        for(int i = 0; i < hexagons.size(); i++){
            std::reverse(hexagons[i].begin(), hexagons[i].end());

            for(int j = 0; j < hexagons[i].size(); j++){
                if(i == 0 && (j == 0 || j == hexagons[i].size() - 1)) hexagons[i][j].setOwner(Owner::PLAYER_A);
                if(i == hexagons.size() - 1 && (j == 0 || j == hexagons[i].size() - 1)) hexagons[i][j].setOwner(Owner::PLAYER_B);
            }
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