cmake_minimum_required(VERSION 3.20)
project(ImageCombiner)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

set(SFML_VERSION "2.5.1")

FetchContent_Declare(
        sfml
        GIT_REPOSITORY "https://github.com/SFML/SFML.git"
        GIT_TAG        "${SFML_VERSION}"
)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics)

if(WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(WIN_SFML_DLL_FILES sfml-graphics-d-2.dll sfml-system-d-2.dll sfml-window-d-2.dll)
    else()
        set(WIN_SFML_DLL_FILES sfml-graphics-2.dll sfml-system-2.dll sfml-window-2.dll)
    endif()
    foreach(FILE IN LISTS WIN_SFML_DLL_FILES)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/_deps/sfml-build/lib/${FILE} ${CMAKE_CURRENT_BINARY_DIR})
    endforeach()
endif()