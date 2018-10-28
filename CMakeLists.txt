cmake_minimum_required(VERSION 3.2)
project(SFMLDemo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11  -I/usr/local/include")

set(SOURCE_FILES main.cpp piece.cpp common.cpp lobby.cpp game.cpp network/client.cpp network/data.cpp network/connection.cpp network/server.cpp network/server.h network/connection.h network/client.h keyboard.cpp keyboard.h piece.h)
add_executable(SFMLDemo ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    link_directories(${SFML_LIBRARY_DIRS})
    target_link_libraries(SFMLDemo ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
endif()

include_directories(/usr/local/include)
