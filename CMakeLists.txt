cmake_minimum_required(VERSION  3.16)
project(postgres)

add_subdirectory(src/bin/psql)

add_executable(main main.cpp)