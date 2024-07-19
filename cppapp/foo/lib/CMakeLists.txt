cmake_minimum_required(VERSION 3.10.2)
project(foo)

set(PROJECT_ROOT ../foo)
set(
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_ROOT}/lib/
)

# bar library
set(LIB_NAME "bar")
file(GLOB LIB_SRC "${LIB_NAME}/src/*.cpp")
add_library(${LIB_NAME} SHARED ${LIB_SRC})
target_include_directories( ${LIB_NAME} PUBLIC
    ${PROJECT_ROOT}/lib/bar/include
)

# bar_tool library
set(LIB_NAME "bar_tool")
file(GLOB LIB_SRC "${LIB_NAME}/src/*.cpp")
add_library(${LIB_NAME} SHARED ${LIB_SRC})
target_include_directories( ${LIB_NAME} PUBLIC
    ${PROJECT_ROOT}/lib/bar_tool/include
)
target_link_libraries(
    ${LIB_NAME} PUBLIC
    bar
)
