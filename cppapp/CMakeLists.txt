cmake_minimum_required(VERSION 3.1)

project(abc)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

IF(MSVC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c17")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c17")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c++17")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Zi /Ob0 /Od /RTC1 /std:c++17")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /O2 /Ob2 /DNDEBUG /std:c++17")
ELSE(MSVC) ## xNIX, MSYS
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c17")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -O0 -std=c17")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++20")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -std=c++2a")
ENDIF(MSVC)

# find dependencies
IF(MSVC)

	include_directories($ENV{INCLUDE})

ELSE (MSVC)

    # gcc `pkg-config --cflags gtk+-3.0` -o hello-world-gtk hello-world-gtk.c `pkg-config --libs gtk+-3.0`
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk4)
    
    include_directories(${GTK_INCLUDE_DIRS})
    link_directories(${GTK_LIBRARY_DIRS})
    add_definitions(${GTK_CFLAGS_OTHER})
    add_compile_options(${GTK_CFLAGS_OTHER})

ENDIF(MSVC)

include_directories(.)

add_executable(a a.cpp)
add_executable(b b.cpp)
add_executable(d d.cpp)
add_executable(tt tt.cpp)
add_executable(trie_sum trie_sum.cpp)
add_executable(colormenu colormenu.cpp)
add_executable(scoville scoville.cpp)

IF(MSYS)

	target_link_libraries(a pthread ws2_32 zmq)
	target_link_libraries(b pthread ws2_32 zmq)
	target_link_libraries(d pthread ws2_32 zmq)
	target_link_libraries(tt pthread ws2_32 zmq)
	target_link_libraries(trie_sum pthread ws2_32 zmq)
	target_link_libraries(colormenu pthread ws2_32 zmq)

ELSEIF(MSVC)
	
	target_link_libraries(a ws2_32)
	target_link_libraries(b ws2_32)
	target_link_libraries(d ws2_32)
	target_link_libraries(tt ws2_32)
	target_link_libraries(trie_sum ws2_32)
	target_link_libraries(colormenu ws2_32)

#target_include_directories(hello_g PRIVATE ${GTK_INCLUDE_DIRS})

ELSE(MSYS)
	target_link_libraries(a pthread)
	target_link_libraries(b pthread)
	target_link_libraries(d pthread)
	target_link_libraries(tt pthread)
	target_link_libraries(trie_sum pthread )
	target_link_libraries(colormenu pthread )

ENDIF(MSYS)

if(MSYS)

#target_compile_definitions(hello_g PRIVATE ${GTK_CFLAGS_OTHER})
#target_include_directories(hello_g PRIVATE ${GTK_INCLUDE_DIRS})
#target_link_directories(hello_g PRIVATE ${GTK_LIBRARY_DIRS})
#target_link_libraries(hello_g PRIVATE ${GTK_LIBRARIES} ws2_32 zmq)

endif()

IF (MSYS OR MSVC)

	install(TARGETS trie_sum DESTINATION C:\\Programs\\bin)
	install(TARGETS tt DESTINATION C:\\Programs\\bin)
	install(TARGETS a DESTINATION C:\\Programs\\bin)
	install(TARGETS b DESTINATION C:\\Programs\\bin)
	install(TARGETS d DESTINATION C:\\Programs\\bin)

ELSE (MSYS OR MSVC)

	install(TARGETS trie_sum DESTINATION ~/bin)
	install(TARGETS tt DESTINATION ~/bin)
	install(TARGETS a DESTINATION ~/bin)
	install(TARGETS b DESTINATION ~/bin)
	install(TARGETS d DESTINATION ~/bin)

ENDIF(MSYS OR MSVC)
