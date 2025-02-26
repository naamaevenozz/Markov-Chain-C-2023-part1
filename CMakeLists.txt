cmake_minimum_required(VERSION 3.28)
project(ex3a_naamaevenoz C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(ex3a_naamaevenoz
        linked_list.c
        linked_list.h
        markov_chain.c
        markov_chain.h
        tweets_generator.c)
