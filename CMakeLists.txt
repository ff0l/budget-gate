cmake_minimum_required(VERSION 3.20)
project(budget-gate LANGUAGES CXX)

add_library(budget_gate STATIC
    src/priority.cpp
    src/gate.cpp
)

target_include_directories(budget_gate PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(budget_gate PUBLIC cxx_std_20)

if(WIN32)
    target_compile_definitions(budget_gate PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()
