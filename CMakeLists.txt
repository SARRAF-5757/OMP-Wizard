cmake_minimum_required(VERSION 3.14)
project(omp-wizard)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

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

# --- Build Executables ---
# Release build
add_executable(omp-wizard main.cpp)
target_compile_options(omp-wizard PRIVATE -O2)
set_target_properties(omp-wizard PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Debug build
add_executable(omp-wizard_debug main.cpp)
target_compile_options(omp-wizard_debug PRIVATE -g -O0)
set_target_properties(omp-wizard_debug PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    COMPILE_DEFINITIONS "DEBUG=1"
)

# --- Link libraries ---
# Release version
target_link_libraries(
    omp-wizard
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE nlohmann_json::nlohmann_json
)

# Debug version
target_link_libraries(
    omp-wizard_debug
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    PRIVATE nlohmann_json::nlohmann_json
)
