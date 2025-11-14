cmake_minimum_required(VERSION 3.28)

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

set(CMAKE_CXX_STANDARD 20)

project(vault)

add_executable(vault vault.cpp)

include_directories(primitives)

target_link_libraries(vault
    PRIVATE
        # Chose a submodule
        ftxui::component
        ftxui::dom
        ftxui::screen
)

target_compile_options(vault 
    PRIVATE 
        -Wall -Werror
)

