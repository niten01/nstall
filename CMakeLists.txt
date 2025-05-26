cmake_minimum_required(VERSION 3.22)

set(NSTALL_VERSION_MAJOR 0)
set(NSTALL_VERSION_MINOR 1)

project(nstall VERSION ${NSTALL_VERSION_MAJOR}.${NSTALL_VERSION_MINOR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)
option(NSTALL_TEST_BUILD "Build with automatic gui testing" OFF)

if(NSTALL_TEST_BUILD)
  set(NANA_CMAKE_AUTOMATIC_GUI_TESTING ON)
endif()


include_directories(include)

set(INSTALLER_TARGET ${PROJECT_NAME}-installer)
set(CONSTRUCTOR_TARGET ${PROJECT_NAME}-constructor)
set(COMMON_TARGET ${PROJECT_NAME}-common)
set(NSTALL_CARRIER_NAME ${PROJECT_NAME}-carrier)

set(INSTALLER_SOURCES
    src/Installer/InstallerMain.cpp src/Installer/PayloadExtractor.cpp
    src/Installer/Installer.cpp)
set(CONSTRUCTOR_SOURCES
  src/Consturctor/ConstructorMain.cpp src/Consturctor/ConstructorForm.cpp
    src/Consturctor/PayloadPacker.cpp)
  set(SHARED_SOURCES src/Common/Payload.cpp)

add_library(${COMMON_TARGET} ${SHARED_SOURCES})
target_compile_definitions(
  ${COMMON_TARGET}
  PUBLIC -DNSTALL_VERSION="${NSTALL_VERSION_MAJOR}.${NSTALL_VERSION_MINOR}"
         -DNSTALL_EXE_EXTENSION="${CMAKE_EXECUTABLE_SUFFIX}"
         -DNSTALL_CARRIER_NAME="${NSTALL_CARRIER_NAME}")

# Dependencies
include(FetchContent)

# miniz
message(STATUS "Fetching dependency: miniz")
FetchContent_Declare(
  miniz
  GIT_REPOSITORY https://github.com/niten01/miniz
  GIT_TAG 41437f08cd8b27055a37583232a3617cff761f4c)
FetchContent_MakeAvailable(miniz)
target_link_libraries(${COMMON_TARGET} PUBLIC miniz)

# nana
message(STATUS "Fetching dependency: nana")
FetchContent_Declare(
  nana
  GIT_REPOSITORY https://github.com/cnjinhao/nana
  GIT_TAG 96d48b8413866028f5d284ee55b763ceab7aeb1f)
FetchContent_MakeAvailable(nana)
set_target_properties(nana PROPERTIES CXX_STANDARD 17)
target_link_libraries(${COMMON_TARGET} PUBLIC nana)

message(STATUS "Fetching dependency: sodium")
FetchContent_Declare(
  Sodium
  GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
  GIT_TAG e5b985ad0dd235d8c4307ea3a385b45e76c74c6a)
set(SODIUM_DISABLE_TESTS ON)
FetchContent_MakeAvailable(Sodium)

target_link_libraries(${COMMON_TARGET} PUBLIC sodium)

add_executable(${INSTALLER_TARGET} ${INSTALLER_SOURCES})
target_link_libraries(${INSTALLER_TARGET} PRIVATE ${COMMON_TARGET})
add_executable(${CONSTRUCTOR_TARGET} ${CONSTRUCTOR_SOURCES})
target_link_libraries(${CONSTRUCTOR_TARGET} PRIVATE ${COMMON_TARGET})

add_custom_command(
  TARGET ${CONSTRUCTOR_TARGET}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
          $<TARGET_FILE_DIR:${CONSTRUCTOR_TARGET}>/resources/
  COMMAND
    ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${INSTALLER_TARGET}>
    $<TARGET_FILE_DIR:${CONSTRUCTOR_TARGET}>/resources/${NSTALL_CARRIER_NAME})
