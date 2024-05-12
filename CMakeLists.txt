cmake_minimum_required(VERSION 3.5)

project(YADRO_task LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_executable(
    YADRO_task
    main.cpp
    event.h
    event.cpp
    computerclub.h
    computerclub.cpp
)
