cmake_minimum_required(VERSION 3.0)

project(Hamming)
set(SOURCES "main.c")
set(EXECUTABLE hamming)

add_executable(${EXECUTABLE} ${SOURCES})
