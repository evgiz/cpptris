cmake_minimum_required(VERSION 3.2)
project(cpptris)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -I/usr/local/include")

set(SOURCE_FILES src/main.cpp src/piece.cpp src/common.cpp src/lobby.cpp src/game.cpp src/network/client.cpp src/network/connection.cpp src/network/server.cpp src/network/server.h src/network/connection.h src/network/client.h src/keyboard.cpp src/keyboard.h src/piece.h)
add_executable(cpptris ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    link_directories(${SFML_LIBRARY_DIRS})
    target_link_libraries(cpptris ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

include_directories(/usr/local/include)
