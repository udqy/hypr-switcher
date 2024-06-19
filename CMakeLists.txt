cmake_minimum_required(VERSION 3.10)

# Set the project name
project(hypr-switcher VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(hypr-switcher src/main.cpp)

# Find and link nlohmann/json
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.10.5 # Or the latest version
)
FetchContent_MakeAvailable(json)
target_link_libraries(hypr-switcher nlohmann_json::nlohmann_json)
