cmake_minimum_required(VERSION 3.16)
project(test_omp)

find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
