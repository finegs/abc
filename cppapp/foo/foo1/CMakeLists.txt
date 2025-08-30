cmake_minimum_required(VERSION 3.10.2)
project(foo)

set(PROJECT_ROOT /home/user01/dev/git/abc/cppapp/foo)
set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT}/bin/
)

add_subdirectory(${PROJECT_ROOT}/lib/ bin)

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(
    ${PROJECT_NAME}
    bar_tool
)
