cmake_minimum_required(VERSION 3.25)
project(Hexxagon)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.1.1
)
FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.0
)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(sfml)
add_executable(Hexxagon
        src/Game.cpp
        src/Main.cpp
        src/Hexagon.cpp
        src/Counter.cpp
        src/Board.cpp
        src/PauseMenu.cpp
        src/Menu.cpp
        src/SavedGamesMenu.cpp)
target_link_libraries(
        Hexxagon
        fmt
        sfml-graphics
        sfml-window
        sfml-system
)
IF (WIN32)
    add_custom_command(TARGET Hexxagon POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Hexxagon> $<TARGET_FILE_DIR:Hexxagon>
            COMMAND_EXPAND_LISTS
    )
ENDIF ()