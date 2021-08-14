cmake_minimum_required(VERSION 3.1)
project(abc)

#add_definitions(-Wall -std=c++2a)

#set(CMAKE_C_FLAGS "-std=c99")
IF(MSYS)
    set(CMAKE_C_FLAGS "-std=c17")
    set(CMAKE_CXX_FLAGS "-std=c++2a")
ELSE(MSYS)
    set(CMAKE_C_FLAGS "-std=c11")
    set(CMAKE_CXX_FLAGS "-std=c++17")
ENDIF(MSYS)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find dependencies
if (MSYS)

    # gcc `pkg-config --cflags gtk+-3.0` -o hello-world-gtk hello-world-gtk.c `pkg-config --libs gtk+-3.0`
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-3.0)
    
    include_directories(${GTK_INCLUDE_DIRS})
    link_directories(${GTK_LIBRARY_DIRS})
    add_definitions(${GTK_CFLAGS_OTHER})
    add_compile_options(${GTK_CFLAGS_OTHER})

endif(MSYS)

#set(LIBRARY ${LIBRARIES} ${GTK_LIBRARIES})
#set(FLAGS "-I{GTK_INCLUDE_DIRS}")
#message(STATUS "Flags: ${FLAGS}")
#string(REPLACE ";" " -I" FLAGS "${FLAGS}")
#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${GTK_FLAGS} ${FLAGS})

#include_directories(D:\\Programs\\wxwidgets\\wxwidgets\\include)
#link_directories(D:\\Programs\\wxwidgets\\wxwidgets\\lib)
include_directories(.)

add_executable(a a.c)
add_executable(ap a.cpp Util.cpp)
add_executable(a1 a1.cpp)
add_executable(a2 a2.cpp)
add_executable(a3 a3.cpp)
add_executable(a4 a4.cpp)
add_executable(b b.c Book.c)
add_executable(zecho zecho.cpp Util.cpp)
add_executable(zcli zcli.cpp)
#add_executable(serial serial.cpp)
add_executable(zsrv z.cpp)
add_executable(aa aa.cpp Util.cpp)
add_executable(c20 c20.cpp)
add_executable(app a.cpp Util.cpp)
add_executable(ThreadPool ThreadPool.cpp)
add_executable(hello_g hello_g.c)
add_executable(a5 a5.cpp)
add_executable(aaa aaa.cpp)

IF(MSYS)
target_link_libraries(a pthread ws2_32 )
target_link_libraries(a1 pthread ws2_32 zmq)
target_link_libraries(a2 pthread ws2_32 )
target_link_libraries(zecho pthread ws2_32 zmq)
target_link_libraries(zcli pthread ws2_32 zmq)
#target_link_libraries(zcli pthread ws2_32 zmq)
#target_link_libraries(serial pthread ws2_32 zmq)
target_link_libraries(zsrv pthread ws2_32 zmq)
target_link_libraries(c20 pthread ws2_32 zmq)
target_link_libraries(app pthread ws2_32 zmq)
target_link_libraries(ThreadPool pthread ws2_32 zmq)
ELSE(MSYS)
target_link_libraries(a pthread )
target_link_libraries(a1 pthread zmq)
target_link_libraries(a2 pthread )
target_link_libraries(zecho pthread zmq)
target_link_libraries(zcli pthread zmq)
#target_link_libraries(zcli pthread zmq)
#target_link_libraries(serial pthread zmq)
target_link_libraries(zsrv pthread zmq)
target_link_libraries(c20 pthread zmq)
target_link_libraries(app pthread zmq)
target_link_libraries(ThreadPool pthread zmq)

ENDIF(MSYS)

if(MSYS)

target_compile_definitions(hello_g PRIVATE ${GTK_CFLAGS_OTHER})
target_include_directories(hello_g PRIVATE ${GTK_INCLUDE_DIRS})
target_link_directories(hello_g PRIVATE ${GTK_LIBRARY_DIRS})
target_link_libraries(hello_g PRIVATE ${GTK_LIBRARIES} ws2_32 zmq)

endif()

#install(TARGETS a 
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib/static
#		)

IF (MSYS)
install(TARGETS a DESTINATION C:\\Programs\\bin)
install(TARGETS a1 DESTINATION C:\\Programs\\bin)
install(TARGETS a2 DESTINATION C:\\Programs\\bin)
install(TARGETS zecho DESTINATION C:\\Programs\\bin)
#install(TARGETS zcli DESTINATION C:\\Programs\\bin)
#install(TARGETS serial DESTINATION C:\\Programs\\bin)
install(TARGETS zsrv DESTINATION C:\\Programs\\bin)
install(TARGETS aa DESTINATION C:\\Programs\\bin)
install(TARGETS c20 DESTINATION C:\\Programs\\bin)
install(TARGETS ThreadPool DESTINATION C:\\Programs\\bin)
ELSE ()
install(TARGETS a DESTINATION ~/bin)
install(TARGETS a1 DESTINATION ~/bin)
install(TARGETS a2 DESTINATION ~/bin)
install(TARGETS zecho DESTINATION ~/bin)
#install(TARGETS zcli DESTINATION ~/bin)
#install(TARGETS serial DESTINATION ~/bin)
install(TARGETS zsrv DESTINATION ~/bin)
install(TARGETS aa DESTINATION ~/bin)
install(TARGETS c20 DESTINATION ~/bin)
install(TARGETS ThreadPool DESTINATION ~/bin)

ENDIF(MSYS)
