cmake_minimum_required(VERSION 3.14)
project(allocater VERSION 1.0 DESCRIPTION "A9" LANGUAGES C)
include_directories(include)
add_executable(main src/main.c src/alloc.c)


