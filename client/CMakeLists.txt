cmake_minimum_required(VERSION 3.7.0)
project(client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS src/forms)
if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(${PROJECT_NAME}_SOURCES
    include/mainwindow.h
    src/main.cpp
    src/mainwindow.cpp
)

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SOURCES}
)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    ${CMAKE_REQUIRED_LIBRARIES}
    ${ALL_LIBRARIES}
)
