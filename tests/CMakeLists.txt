set(PARENT_PROJECT_NAME ${PROJECT_NAME})
project(${PARENT_PROJECT_NAME}_tests)

include(FetchContent)

enable_testing()

find_package(Python REQUIRED COMPONENTS Interpreter)

set(E2E_PROJECT_NAME ${PROJECT_NAME}_e2e)
add_test(NAME ${E2E_PROJECT_NAME}_small
         COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/e2e.py
         $<TARGET_FILE:${CONSTRUCTOR_TARGET}> ${CMAKE_SOURCE_DIR}/src)

add_test(NAME ${E2E_PROJECT_NAME}_big
         COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/e2e.py
         $<TARGET_FILE:${CONSTRUCTOR_TARGET}>)
