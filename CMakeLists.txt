cmake_minimum_required(VERSION  3.16)
project(postgres)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src/bin/psql)

add_executable(main main.cpp)