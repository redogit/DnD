cmake_minimum_required(VERSION 3.25)
project(decision_field LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(decision_field INTERFACE)
target_include_directories(decision_field INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(decision_field INTERFACE cxx_std_23)

if (MSVC)
  target_compile_options(decision_field INTERFACE /W4 /permissive-)
else()
  target_compile_options(decision_field INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_executable(decision_field_demo examples/demo.cpp)
target_link_libraries(decision_field_demo PRIVATE decision_field)

add_executable(decision_field_tests tests/decision_field_tests.cpp)
target_link_libraries(decision_field_tests PRIVATE decision_field)

enable_testing()
add_test(NAME decision_field_tests COMMAND decision_field_tests)

add_executable(decision_field_self_opposite examples/self_opposite.cpp)
target_link_libraries(decision_field_self_opposite PRIVATE decision_field)
add_test(NAME decision_field_self_opposite COMMAND decision_field_self_opposite)

add_executable(decision_field_triadic_self_attack examples/triadic_self_attack.cpp)
target_link_libraries(decision_field_triadic_self_attack PRIVATE decision_field)
add_test(NAME decision_field_triadic_self_attack COMMAND decision_field_triadic_self_attack)

add_executable(decision_field_component_analysis_tests tests/component_analysis_tests.cpp)
target_link_libraries(decision_field_component_analysis_tests PRIVATE decision_field)
add_test(NAME decision_field_component_analysis_tests COMMAND decision_field_component_analysis_tests)
