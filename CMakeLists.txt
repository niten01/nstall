cmake_minimum_required(VERSION 3.22)
project(nstall VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

set(NSTALL_SOURCES src/Main.cpp
src/PayloadExtractor.cpp)

add_executable(${PROJECT_NAME} ${NSTALL_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE -DNSTALL_VERSION="${PROJECT_VERSION}")

# Dependencies
include(FetchContent)

# miniz
message(STATUS "Fetching dependency: miniz")
FetchContent_Declare(
  miniz
  GIT_REPOSITORY https://github.com/niten01/miniz
  GIT_TAG 41437f08cd8b27055a37583232a3617cff761f4c) 
FetchContent_MakeAvailable(miniz)
target_link_libraries(${PROJECT_NAME} PRIVATE miniz)

# spdlog
message(STATUS "Fetching dependency: spdlog")
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG 6fa36017cfd5731d617e1a934f0e5ea9c4445b13) # 11.0.2
FetchContent_MakeAvailable(spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

# fltk
# message(STATUS "Fetching dependency: fltk")
# FetchContent_Declare(
#   fltk
#   GIT_REPOSITORY https://github.com/fltk/fltk
#   GIT_TAG aaabff439306215b3195dff5f2152fe6eadaa323) 
# FetchContent_MakeAvailable(fltk)
# target_link_libraries(${PROJECT_NAME} PRIVATE fltk)

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
target_link_libraries(${PROJECT_NAME} PRIVATE nana)

# # wx
# message(STATUS "Fetching dependency: wxWidgets")
# set(wxBUILD_SHARED OFF)
# set(wxBUILD_PRECOMP OFF)
# set(wxBUILD_MONOLITHIC OFF)
# FetchContent_Declare(
#   wxWidgets
#   GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets
#   GIT_TAG v3.2.8) 
# FetchContent_MakeAvailable(wxWidgets)
# target_link_libraries(${PROJECT_NAME} PRIVATE wx::base wx::core)
