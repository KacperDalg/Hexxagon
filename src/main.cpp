#include "headers/Game.hpp"

int main() {
    auto window = sf::RenderWindow{{1000, 600}, "Hexxagon"};

    Game game(window);
    game.run();

    return 0;
}