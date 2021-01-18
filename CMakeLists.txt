cmake_minimum_required(VERSION 3.18)
project(abc)

add_definitions(-Wall -std=c++2a)

#include_directories(D:\\Programs\\wxwidgets\\wxwidgets\\include)
#link_directories(D:\\Programs\\wxwidgets\\wxwidgets\\lib)

add_executable(a a.cpp)
add_executable(aa aa.c)
add_executable(b b.cpp list.cpp)
add_executable(w w.cpp)

target_link_libraries(a pthread ws2_32 )
target_link_libraries(aa pthread ws2_32 )
target_link_libraries(b pthread ws2_32 )
target_link_libraries(w pthread ws2_32 )

#install(TARGETS a 
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib/static
#		)

install(TARGETS a DESTINATION C:\\Programs\\bin)
install(TARGETS aa DESTINATION C:\\Programs\\bin)
install(TARGETS b DESTINATION C:\\Programs\\bin)
install(TARGETS w DESTINATION C:\\Programs\\bin)
