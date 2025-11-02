cmake_minimum_required(VERSION 3.14)
project(omp-wizard)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# --- FTXUI ---
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

# --- nlohmann/json ---
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

# --- Build Executable ---
add_executable(omp-wizard main.cpp)

# --- Link libraries ---
target_link_libraries(
    omp-wizard
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE nlohmann_json::nlohmann_json
)
