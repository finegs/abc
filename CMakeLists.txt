cmake_minimum_required(VERSION 3.1)
project(abc)

#add_definitions(-Wall -std=c++2a)

set(CMAKE_C_FLAGS "-std=c99")
set(CMAKE_CXX_FLAGS "-std=c++2a")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#include_directories(D:\\Programs\\wxwidgets\\wxwidgets\\include)
#link_directories(D:\\Programs\\wxwidgets\\wxwidgets\\lib)
include_directories(.)

add_executable(a a.c)
add_executable(a1 a1.cpp)
add_executable(a2 a2.cpp)
add_executable(a3 a3.cpp)
add_executable(a4 a4.cpp)
add_executable(b b.c book.c)
add_executable(zecho zecho.cpp)
#add_executable(zcli zcli.cpp)
#add_executable(serial serial.cpp)
add_executable(z z.cpp)
add_executable(aa aa.cpp Util.cpp)
add_executable(c20 c20.cpp)
add_executable(app a.cpp Util.cpp)

target_link_libraries(a pthread ws2_32 )
target_link_libraries(a1 pthread ws2_32 zmq)
target_link_libraries(a2 pthread ws2_32 )
target_link_libraries(zecho pthread ws2_32 zmq)
#target_link_libraries(zcli pthread ws2_32 zmq)
#target_link_libraries(zcli pthread ws2_32 zmq)
#target_link_libraries(serial pthread ws2_32 zmq)
target_link_libraries(z pthread ws2_32 zmq)
target_link_libraries(c20 pthread ws2_32 zmq)
target_link_libraries(app pthread ws2_32 zmq)

#install(TARGETS a 
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib/static
#		)

install(TARGETS a DESTINATION C:\\Programs\\bin)
install(TARGETS a1 DESTINATION C:\\Programs\\bin)
install(TARGETS a2 DESTINATION C:\\Programs\\bin)
install(TARGETS zecho DESTINATION C:\\Programs\\bin)
#install(TARGETS zcli DESTINATION C:\\Programs\\bin)
#install(TARGETS serial DESTINATION C:\\Programs\\bin)
install(TARGETS z DESTINATION C:\\Programs\\bin)
install(TARGETS aa DESTINATION C:\\Programs\\bin)
install(TARGETS c20 DESTINATION C:\\Programs\\bin)

