cmake_minimum_required(VERSION 2.8)
project(vector C)

set(SRCS
    vector.c
    )

add_library(vector ${SRCS})

add_executable(vector_test vector_test.c)
target_link_libraries(vector_test vector)