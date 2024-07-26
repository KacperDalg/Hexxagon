#include "headers/Board.hpp"

Board::Board(int rows, int cols, float hexSize, sf::RenderWindow &window) : rows(rows), cols(cols), hexSize(hexSize),
                                                                            window(window),
                                                                            playerACounter(window, Player::PLAYER_A),
                                                                            playerBCounter(window, Player::PLAYER_B) {}

void Board::start() {
    hexagons.clear();
    initializeHexagons();
    calculatePoints();
    currentPlayer = Player::PLAYER_A;
}

void Board::draw() {
    for (auto &col: hexagons) {
        for (auto &hexagon: col) {
            hexagon.draw();
        }
    }
    playerACounter.draw();
    playerBCounter.draw();
}

void Board::save() {
    std::filesystem::path folderPath = "../saved";

    if (!std::filesystem::exists(folderPath)) {
        std::filesystem::create_directory(folderPath);
    }

    //https://stackoverflow.com/questions/16357999/current-date-and-time-as-string
    auto now = std::time(nullptr);
    auto dateTime = *std::localtime(&now);

    std::stringstream ss;
    ss << std::put_time(&dateTime, "%d-%m-%Y_%H-%M-%S");
    auto dateString = ss.str();

    auto fileName = "../saved/Hexxagon_" + dateString;

    std::fstream file(fileName, std::ios::out);

    if (file.is_open()) {
        file << static_cast<int>(currentPlayer);
        for (int i = 0; i < hexagons.size(); i++) {
            for (int j = 0; j < hexagons[i].size(); j++) {
                file << static_cast<int>(hexagons[i][j].getOwner());
            }
        }
    }
}

void Board::load(std::string const &fileName) {
    start();

    std::fstream file("../saved/" + fileName);

    if (file.is_open()) {
        std::string line;
        if (std::getline(file, line)) {
            if (line.size() == 62) {
                currentPlayer = static_cast<Player>(static_cast<int>(line[0] - '0'));

                int index = 1;
                for (int i = 0; i < hexagons.size(); i++) {
                    for (int j = 0; j < hexagons[i].size(); j++) {
                        hexagons[i][j].setOwner(static_cast<Player>(static_cast<int>(line[index] - '0')));
                        index++;
                    }
                }
            } else {
                throw std::runtime_error("Incorrect file content.");
            }
        } else {
            throw std::runtime_error("File is empty.");
        }
    }
    calculatePoints();
}

void Board::onMouseClick(float mouseX, float mouseY) {
    for (int i = 0; i < hexagons.size(); i++) {
        for (int j = 0; j < hexagons[i].size(); j++) {
            if (hexagons[i][j].containsCoordinates(mouseX, mouseY)) {

                if (hexagons[i][j].getState() == HexagonState::DEFAULT ||
                    hexagons[i][j].getState() == HexagonState::SELECTED) {
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

                    prepareForNextMove();
                    return;
                }

                if (hexagons[i][j].getState() == HexagonState::JUMP_OPTION) {
                    Hexagon &selectedHexagon = getSelectedHexagon();

                    hexagons[i][j].setOwner(selectedHexagon.getOwner());
                    selectedHexagon.setOwner(Player::NO_PLAYER);
                    setAdjacentHexagons(i, j, AdjacentHexagonsMode::TAKE_OVER_MODE);

                    prepareForNextMove();
                    return;
                }
            }
        }
    }
}

void Board::initializeHexagons() {
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

    //ROWS AND COLUMNS SORT + SETTING START POSITION FOR PLAYERS
    for (int i = 0; i < hexagons.size(); i++) {
        std::reverse(hexagons[i].begin(), hexagons[i].end());

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

void Board::changePlayer() {
    if (currentPlayer == Player::PLAYER_A) {
        currentPlayer = Player::PLAYER_B;
    } else {
        currentPlayer = Player::PLAYER_A;
    }
}

Hexagon &Board::getHexagon(int column, int row) {
    for (int i = 0; i < hexagons.size(); i++) {
        for (int j = 0; j < hexagons[i].size(); j++) {
            if (i == column && j == row) {
                return hexagons[i][j];
            }
        }
    }
}

void Board::prepareForNextMove() {
    calculatePoints();
    checkForWinner();
    resetStates();
    changePlayer();
}

Hexagon &Board::getSelectedHexagon() {
    for (auto &col: hexagons) {
        for (auto &hexagon: col) {
            if (hexagon.getState() == HexagonState::SELECTED) {
                return hexagon;
            }
        }
    }
}

std::pair<int, int> Board::getSelectedHexagonBoardPosition() {
    for (int i = 0; i < hexagons.size(); i++) {
        for (int j = 0; j < hexagons[i].size(); j++) {
            if (hexagons[i][j].getState() == HexagonState::SELECTED) {
                return std::make_pair(i, j);
            }
        }
    }
}

void Board::calculatePoints() {
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

void Board::checkForWinner() {
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

void Board::resetStates() {
    for (auto &col: hexagons) {
        for (auto &hexagon: col) {
            hexagon.setState(HexagonState::DEFAULT);
        }
    }
}

void Board::setAdjacentHexagons(int column, int row, AdjacentHexagonsMode mode) {
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

void Board::setHexagonJumpOptions() {
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