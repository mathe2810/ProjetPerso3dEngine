cmake_minimum_required(VERSION 3.23)
project(ProjetPerso C)

set(CMAKE_C_STANDARD 99)

add_executable(ProjetPerso main.c)

target_link_libraries(ProjetPerso -lmingw32 -lalleg44 -lm)
