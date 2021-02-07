cmake_minimum_required(VERSION 3.18)
project(abc)

add_definitions(-Wall -std=c++2a)

#include_directories(D:\\Programs\\wxwidgets\\wxwidgets\\include)
#link_directories(D:\\Programs\\wxwidgets\\wxwidgets\\lib)

add_executable(a a.cpp)
add_executable(b b.c book.c)
add_executable(zecho zecho.cpp)
#add_executable(zcli zcli.cpp)
#add_executable(serial serial.cpp)
add_executable(z z.cpp)
add_executable(aa aa.cpp List.cpp Util.cpp)

target_link_libraries(a pthread ws2_32 )
target_link_libraries(zecho pthread ws2_32 zmq)
#target_link_libraries(zcli pthread ws2_32 zmq)
#target_link_libraries(serial pthread ws2_32 zmq)
target_link_libraries(z pthread ws2_32 zmq)

#install(TARGETS a 
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib/static
#		)

install(TARGETS a DESTINATION C:\\Programs\\bin)
install(TARGETS zecho DESTINATION C:\\Programs\\bin)
#install(TARGETS zcli DESTINATION C:\\Programs\\bin)
#install(TARGETS serial DESTINATION C:\\Programs\\bin)
install(TARGETS z DESTINATION C:\\Programs\\bin)
install(TARGETS aa DESTINATION C:\\Programs\\bin)
