cmake_minimum_required(VERSION 3.1)
project(abc)

#add_definitions(-Wall -std=c++2a)

#set(CMAKE_C_FLAGS "-std=c99")
IF(MSYS)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c17")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -O0 -std=c17")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++2a")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -std=c++2a")
ELSEIF(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c17")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c++17")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Zi /Ob0 /Od /RTC1 /std:c++17")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /O2 /Ob2 /DNDEBUG /std:c++17")
ELSE(MSYS)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c17")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -O0 -std=c17")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++2a")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -std=c++2a")
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

include_directories(.)

add_executable(a a.cpp)
add_executable(d d.cpp)
add_executable(cc cc.cpp)
add_executable(colormenu colormenu.cpp)

IF(MSYS)
target_link_libraries(a pthread ws2_32 zmq)
target_link_libraries(d pthread ws2_32 zmq)
target_link_libraries(cc pthread ws2_32 zmq)
target_link_libraries(colormenu pthread ws2_32 zmq)
ELSEIF(MSVC)
target_link_libraries(a ws2_32)
target_link_libraries(d ws2_32)
target_link_libraries(cc ws2_32)
target_link_libraries(colormenu ws2_32)
ELSE(MSYS)
target_link_libraries(a pthread )
target_link_libraries(d pthread)
target_link_libraries(cc pthread )
target_link_libraries(colormenu pthread )

ENDIF(MSYS)

if(MSYS)

#target_compile_definitions(hello_g PRIVATE ${GTK_CFLAGS_OTHER})
#target_include_directories(hello_g PRIVATE ${GTK_INCLUDE_DIRS})
#target_link_directories(hello_g PRIVATE ${GTK_LIBRARY_DIRS})
#target_link_libraries(hello_g PRIVATE ${GTK_LIBRARIES} ws2_32 zmq)

endif()

#install(TARGETS a 
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION lib/static
#		)

IF (MSYS)

install(TARGETS a DESTINATION C:\\Programs\\bin)
install(TARGETS d DESTINATION C:\\Programs\\bin)

ELSE (MSYS)

install(TARGETS a DESTINATION ~/bin)
install(TARGETS d DESTINATION ~/bin)

ENDIF(MSYS)
