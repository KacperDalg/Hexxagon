#pragma once

#include "Enums.hpp"
#include "Hexagon.hpp"
#include "Counter.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <fstream>

class Board {
public:
    Board(int rows, int cols, float hexSize, sf::RenderWindow &window);

    void start();

    void draw();

    void save();

    void load(std::string const &fileName);

    void onMouseClick(float mouseX, float mouseY);

private:
    int rows, cols, playerAPoints, playerBPoints, emptyFields;
    float hexSize;
    sf::RenderWindow &window;
    std::vector<std::vector<Hexagon>> hexagons;
    Counter playerACounter, playerBCounter;
    Player currentPlayer;

    void initializeHexagons();

    void changePlayer();

    Hexagon &getHexagon(int column, int row);

    void prepareForNextMove();

    Hexagon &getSelectedHexagon();

    std::pair<int, int> getSelectedHexagonBoardPosition();

    void calculatePoints();

    void checkForWinner();

    void resetStates();

    void setAdjacentHexagons(int column, int row, AdjacentHexagonsMode mode);

    void setHexagonJumpOptions();
};