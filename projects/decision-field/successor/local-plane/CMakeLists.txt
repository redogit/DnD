cmake_minimum_required(VERSION 3.25)
project(decision_field_local_plane_successor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

get_filename_component(DF_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

add_library(decision_field_local_plane INTERFACE)
target_include_directories(decision_field_local_plane INTERFACE "${DF_ROOT}/include")
target_compile_features(decision_field_local_plane INTERFACE cxx_std_23)
if(MSVC)
  target_compile_options(decision_field_local_plane INTERFACE /W4 /permissive-)
else()
  target_compile_options(decision_field_local_plane INTERFACE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

enable_testing()
add_executable(decision_field_local_plane_successor_tests "${DF_ROOT}/tests/local_plane_tests.cpp")
target_link_libraries(decision_field_local_plane_successor_tests PRIVATE decision_field_local_plane)
add_test(NAME decision_field_local_plane_successor_tests COMMAND decision_field_local_plane_successor_tests)

option(DF_LOCAL_PLANE_WITH_NATIVE "Also build the AnyFunctor/native local-plane seam against recovered v0.3 source" OFF)
if(DF_LOCAL_PLANE_WITH_NATIVE)
  # Reuse the exact preserved v0.7 build surface rather than editing it. The caller must
  # provide DF_PREDECESSOR_SOURCE exactly as the parent build requires.
  add_subdirectory("${DF_ROOT}" "${CMAKE_BINARY_DIR}/decision-field-v07")
  if(NOT TARGET decision_field_native_persistence)
    message(FATAL_ERROR "Native local-plane seam requires exact recovered v0.3 source and persistence dependencies")
  endif()
  add_executable(decision_field_anyfunctor_local_plane_successor_tests
    "${DF_ROOT}/tests/anyfunctor_local_plane_tests.cpp")
  target_link_libraries(decision_field_anyfunctor_local_plane_successor_tests
    PRIVATE decision_field_native_persistence)
  add_test(NAME decision_field_anyfunctor_local_plane_successor_tests
    COMMAND decision_field_anyfunctor_local_plane_successor_tests)
endif()
