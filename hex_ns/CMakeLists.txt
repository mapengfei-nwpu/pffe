cmake_minimum_required(VERSION 3.5)


set(PROJECT_NAME demo.exe)
project(${PROJECT_NAME})


find_package(DOLFIN REQUIRED)
include(${DOLFIN_USE_FILE})


add_executable(${PROJECT_NAME} navier_stokes.cpp)
target_link_libraries(${PROJECT_NAME} dolfin)

