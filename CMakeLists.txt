cmake_minimum_required(VERSION 3.22)

set(NSTALL_VERSION_MAJOR 0)
set(NSTALL_VERSION_MINOR 1)

project(nstall VERSION ${NSTALL_VERSION_MAJOR}.${NSTALL_VERSION_MINOR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)
option(NSTALL_CLI_ONLY "Build with no GUI" OFF)

include_directories(include)

set(INSTALLER_TARGET ${PROJECT_NAME}-installer)
set(CONSTRUCTOR_TARGET ${PROJECT_NAME}-constructor)
set(COMMON_TARGET ${PROJECT_NAME}-common)
set(NSTALL_CARRIER_NAME ${PROJECT_NAME}-carrier)

set(INSTALLER_SOURCES
    src/Installer/InstallerMain.cpp src/Installer/PayloadExtractor.cpp
    src/Installer/InstallerCLI.cpp)
set(CONSTRUCTOR_SOURCES
    src/Constructor/ConstructorMain.cpp src/Constructor/ConstructorCLI.cpp
    src/Constructor/PayloadPacker.cpp)
set(SHARED_SOURCES src/Common/MetaInfo.cpp)

add_library(${COMMON_TARGET} ${SHARED_SOURCES})
target_compile_definitions(
  ${COMMON_TARGET}
  PUBLIC -DNSTALL_VERSION="${NSTALL_VERSION_MAJOR}.${NSTALL_VERSION_MINOR}"
         -DNSTALL_EXE_EXTENSION="${CMAKE_EXECUTABLE_SUFFIX}"
         -DNSTALL_CARRIER_NAME="${NSTALL_CARRIER_NAME}")

if(NSTALL_CLI_ONLY)
  target_compile_definitions(${COMMON_TARGET} PUBLIC -DNSTALL_CLI_ONLY)
else()
  list(APPEND INSTALLER_SOURCES src/Installer/InstallerForm.cpp)
  list(APPEND CONSTRUCTOR_SOURCES src/Constructor/ConstructorForm.cpp)
endif()

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

# cxxopts
message(STATUS "Fetching dependency: cxxopts")
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts
  GIT_TAG 44380e5a44706ab7347f400698c703eb2a196202)
FetchContent_MakeAvailable(cxxopts)
target_link_libraries(${COMMON_TARGET} PUBLIC cxxopts)

# fmt
message(STATUS "Fetching dependency: fmt")
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 0c9fce2ffefecfdce794e1859584e25877b7b592) # 11.0.2
FetchContent_MakeAvailable(fmt)
target_link_libraries(${COMMON_TARGET} PUBLIC fmt::fmt)

if(NOT NSTALL_CLI_ONLY)
  # nana
  message(STATUS "Fetching dependency: nana")
  FetchContent_Declare(
    nana
    GIT_REPOSITORY https://github.com/cnjinhao/nana
    GIT_TAG 96d48b8413866028f5d284ee55b763ceab7aeb1f)
  FetchContent_MakeAvailable(nana)
  set_target_properties(nana PROPERTIES CXX_STANDARD 17)
  target_link_libraries(${COMMON_TARGET} PUBLIC nana)
endif()

# libsodium
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
  TARGET ${INSTALLER_TARGET}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
          $<TARGET_FILE_DIR:${CONSTRUCTOR_TARGET}>/resources/
  COMMAND
    ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${INSTALLER_TARGET}>
    $<TARGET_FILE_DIR:${CONSTRUCTOR_TARGET}>/resources/${NSTALL_CARRIER_NAME})

add_subdirectory(tests)
