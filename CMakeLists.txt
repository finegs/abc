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

add_executable(a a.cpp)
add_executable(zcli zcli.cpp)
add_executable(zsrv zecho.cpp)
add_executable(d d.cpp)

IF(MSYS)
target_link_libraries(a pthread ws2_32 )
target_link_libraries(zcli pthread ws2_32 zmq)
target_link_libraries(zsrv pthread ws2_32 zmq)
target_link_libraries(d pthread ws2_32 zmq)
ELSE(MSYS)
target_link_libraries(a pthread )
target_link_libraries(zcli pthread zmq)
target_link_libraries(zsrv pthread zmq)
target_link_libraries(d pthread zmq)

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
install(TARGETS d DESTINATION C:\\Programs\\bin)
ELSE ()
install(TARGETS a DESTINATION ~/bin)
install(TARGETS d DESTINATION ~/bin)

ENDIF(MSYS)
