cmake_minimum_required(VERSION 3.22)
project(nstall VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF)

include_directories(include)

set(INSTALLER_TARGET ${PROJECT_NAME}-installer)
set(CONSTRUCTOR_TARGET ${PROJECT_NAME}-constructor)
set(SHARED_TARGET ${PROJECT_NAME}-shared)

set(INSTALLER_SOURCES 
  src/Installer/InstallerMain.cpp
  src/Installer/PayloadExtractor.cpp
  src/Installer/Installer.cpp
)
set(CONSTRUCTOR_SOURCES 
  src/Consturctor/ConstructorMain.cpp
  src/Consturctor/Constructor.cpp
)
set(SHARED_SOURCES 
  src/Shared/Payload.cpp
)

add_library(${SHARED_TARGET} ${SHARED_SOURCES})
target_compile_definitions(${SHARED_TARGET} PUBLIC -DNSTALL_VERSION="${PROJECT_VERSION}")

# Dependencies
include(FetchContent)

# miniz
message(STATUS "Fetching dependency: miniz")
FetchContent_Declare(
  miniz
  GIT_REPOSITORY https://github.com/niten01/miniz
  GIT_TAG 41437f08cd8b27055a37583232a3617cff761f4c) 
FetchContent_MakeAvailable(miniz)
target_link_libraries(${SHARED_TARGET} PUBLIC miniz)

# spdlog
message(STATUS "Fetching dependency: spdlog")
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG 6fa36017cfd5731d617e1a934f0e5ea9c4445b13) # 11.0.2
FetchContent_MakeAvailable(spdlog)
target_link_libraries(${SHARED_TARGET} PUBLIC spdlog)

# nana
message(STATUS "Fetching dependency: nana")
FetchContent_Declare(
  nana
  GIT_REPOSITORY https://github.com/cnjinhao/nana
  GIT_TAG 96d48b8413866028f5d284ee55b763ceab7aeb1f) 
FetchContent_MakeAvailable(nana)
set_target_properties(nana PROPERTIES
    CXX_STANDARD 17
)
target_link_libraries(${SHARED_TARGET} PUBLIC nana)

message(STATUS "Fetching dependency: sodium")
FetchContent_Declare(Sodium
  GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
  GIT_TAG e5b985ad0dd235d8c4307ea3a385b45e76c74c6a
)
set(SODIUM_DISABLE_TESTS ON)
FetchContent_MakeAvailable(Sodium)

target_link_libraries(${SHARED_TARGET} PUBLIC sodium)


add_executable(${INSTALLER_TARGET} ${INSTALLER_SOURCES})
target_link_libraries(${INSTALLER_TARGET} PRIVATE ${SHARED_TARGET})
add_executable(${CONSTRUCTOR_TARGET} ${INSTALLER_SOURCES})
target_link_libraries(${CONSTRUCTOR_TARGET} PRIVATE ${SHARED_TARGET})
