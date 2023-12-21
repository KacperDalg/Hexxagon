cmake_minimum_required(VERSION 3.26.4)
project(CMakeSFMLProject)
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
add_executable(CMakeSFMLProject src/main.cpp)
configure_file(fonts/Silkscreen-Regular.ttf ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
target_link_libraries(
        CMakeSFMLProject
        fmt
        sfml-graphics
        sfml-window
        sfml-system
        stdc++fs
)
IF (WIN32)
    add_custom_command(TARGET CMakeSFMLProject POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:CMakeSFMLProject> $<TARGET_FILE_DIR:CMakeSFMLProject>
            COMMAND_EXPAND_LISTS
    )
ENDIF ()