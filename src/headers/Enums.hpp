#pragma once

enum class Player {
    NO_PLAYER,
    PLAYER_A,
    PLAYER_B
};

enum class HexagonState {
    DEFAULT,
    CLONE_OPTION,
    JUMP_OPTION,
    SELECTED
};

enum class AdjacentHexagonsMode {
    TAKE_OVER_MODE,
    CLONE_OPTIONS_VIEW_MODE,
};

enum class GameState {
    Menu,
    Game,
    Paused,
    SavedGamesMenu
};