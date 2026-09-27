cmake_minimum_required(VERSION 3.25)
project(decision_field_component_analysis LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(decision_field_component_analysis INTERFACE)
target_include_directories(decision_field_component_analysis INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(decision_field_component_analysis INTERFACE cxx_std_23)

if (MSVC)
  target_compile_options(decision_field_component_analysis INTERFACE /W4 /permissive-)
else()
  target_compile_options(decision_field_component_analysis INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_executable(decision_field_component_analysis_tests tests/component_analysis_tests.cpp)
target_link_libraries(decision_field_component_analysis_tests PRIVATE decision_field_component_analysis)

enable_testing()
add_test(NAME decision_field_component_analysis_tests COMMAND decision_field_component_analysis_tests)
