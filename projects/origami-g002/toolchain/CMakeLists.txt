cmake_minimum_required(VERSION 3.25)
project(RMAL VERSION 3.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(rmal STATIC src/rmal.c)
target_include_directories(rmal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(rmalc src/main.c)
target_link_libraries(rmalc PRIVATE rmal)

if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(rmal PRIVATE -Wall -Wextra -Wpedantic -Werror)
  target_compile_options(rmalc PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
  target_compile_options(rmal PRIVATE /W4 /WX)
  target_compile_options(rmalc PRIVATE /W4 /WX)
endif()

include(GNUInstallDirs)

install(TARGETS rmal rmalc
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY spec/ DESTINATION ${CMAKE_INSTALL_DATADIR}/rmal/spec)
install(DIRECTORY examples/ DESTINATION ${CMAKE_INSTALL_DATADIR}/rmal/examples
  FILES_MATCHING PATTERN "*.rmal"
)
install(FILES README.md README_COMPILER.md CURRENT.md
  DESTINATION ${CMAKE_INSTALL_DATADIR}/rmal
)

set(CPACK_PACKAGE_NAME "RMAL")
set(CPACK_PACKAGE_VENDOR "redogit")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RMAL and RMALC native ISO C23 toolchain")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "ZIP")
include(CPack)

include(CTest)
if(BUILD_TESTING)
  add_executable(rmal_numeric_host_test tests/numeric-host.c)
  target_link_libraries(rmal_numeric_host_test PRIVATE rmal)
  add_test(NAME rmal_numeric_host COMMAND rmal_numeric_host_test)
  add_test(NAME rmal_numeric_extension COMMAND rmalc run ${CMAKE_CURRENT_SOURCE_DIR}/tests/numeric-extension.rmal)
  add_test(NAME rmal_selfcheck COMMAND rmalc selfcheck)
  add_test(NAME rmal_language_spec COMMAND rmalc language-spec --format json)
  add_test(NAME rmal_example_check COMMAND rmalc check ${CMAKE_CURRENT_SOURCE_DIR}/examples/hello.rmal)
  add_test(NAME rmal_example_run COMMAND rmalc run ${CMAKE_CURRENT_SOURCE_DIR}/examples/hello.rmal)
  add_test(NAME rmal_recovered_surface COMMAND rmalc check ${CMAKE_CURRENT_SOURCE_DIR}/tests/recovered_surface.rmal)
  add_test(NAME rmal_const_immutable COMMAND rmalc run ${CMAKE_CURRENT_SOURCE_DIR}/tests/const_immutable_reject.rmal)
  set_tests_properties(rmal_const_immutable PROPERTIES WILL_FAIL TRUE)
  add_test(NAME rmal_typed_equality COMMAND rmalc run ${CMAKE_CURRENT_SOURCE_DIR}/tests/typed_equality.rmal)
endif()
