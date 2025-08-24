CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(abc)

SET(CMAKE_BUILD_TYPE Debug)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# for source path
SET(PJT_SRC_DIR src)


IF(MSVC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c17")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c17")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MDd /Zi /Ob0 /Od /RTC1 /std:c++17")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd \
                                                      /Zi \
                                                      /Ob0 \
                                                      /Od \
                                                      /RTC1 \
                                                      /std:c++17")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD \
                                                      /O2 \
                                                      /Ob2 \
                                                      /DNDEBUG \
                                                      /std:c++17")
ELSE() ## xNIX, MSYS
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c17")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -O0 -std=c17")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++20")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -std=c++2a")
ENDIF()

# find dependencies
IF(MSVC)

  INCLUDE_DIRECTORIES($ENV{INCLUDE})

ELSE()

  # gcc `pkg-config --cflags gtk+-3.0` -o hello-world-gtk \
  # hello-world-gtk.c `pkg-config --libs gtk+-3.0`
  FIND_PACKAGE(PkgConfig REQUIRED)
  PKG_CHECK_MODULES(GTK REQUIRED gtk4)

  INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIRS})
  LINK_DIRECTORIES(${GTK_LIBRARY_DIRS})
  ADD_DEFINITIONS(${GTK_CFLAGS_OTHER})
  ADD_COMPILE_OPTIONS(${GTK_CFLAGS_OTHER})

ENDIF()

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/inc)

ADD_EXECUTABLE(a          ${PJT_SRC_DIR}/a.cpp)
ADD_EXECUTABLE(b          ${PJT_SRC_DIR}/b.cpp)
ADD_EXECUTABLE(d          ${PJT_SRC_DIR}/d.cpp)
ADD_EXECUTABLE(tt         ${PJT_SRC_DIR}/tt.cpp)
ADD_EXECUTABLE(trie_sum   ${PJT_SRC_DIR}/trie_sum.cpp)
ADD_EXECUTABLE(colormenu  ${PJT_SRC_DIR}/colormenu.cpp)
ADD_EXECUTABLE(scoville   ${PJT_SRC_DIR}/scoville.cpp)

ADD_EXECUTABLE(algo       ${PJT_SRC_DIR}/algo.cpp)

ADD_EXECUTABLE(dijstra ${PJT_SRC_DIR}/dijstra.cpp)

ADD_SUBDIRECTORY(tests)

IF(MSYS)

  TARGET_LINK_LIBRARIES(a pthread ws2_32 zmq)
  TARGET_LINK_LIBRARIES(b pthread ws2_32 zmq)
  TARGET_LINK_LIBRARIES(d pthread ws2_32 zmq)
  TARGET_LINK_LIBRARIES(tt pthread ws2_32 zmq)
  TARGET_LINK_LIBRARIES(trie_sum pthread ws2_32 zmq)
  TARGET_LINK_LIBRARIES(colormenu pthread ws2_32 zmq)

ELSEIF(MSVC)

  TARGET_LINK_LIBRARIES(a ws2_32)
  TARGET_LINK_LIBRARIES(b ws2_32)
  TARGET_LINK_LIBRARIES(d ws2_32)
  TARGET_LINK_LIBRARIES(tt ws2_32)
  TARGET_LINK_LIBRARIES(trie_sum ws2_32)
  TARGET_LINK_LIBRARIES(colormenu ws2_32)

  #TARGET_INCLUDE_DIRECTORIES(hello_g PRIVATE ${GTK_INCLUDE_DIRS})

ELSE()
  TARGET_LINK_LIBRARIES(a pthread)
  TARGET_LINK_LIBRARIES(b pthread)
  TARGET_LINK_LIBRARIES(d pthread)
  TARGET_LINK_LIBRARIES(tt pthread)
  TARGET_LINK_LIBRARIES(trie_sum pthread)
  TARGET_LINK_LIBRARIES(colormenu pthread)

ENDIF()

IF(MSYS)

  #TARGET_COMPILE_DEFINITIONS(hello_g PRIVATE ${GTK_CFLAGS_OTHER})
  #TARGET_INCLUDE_DIRECTORIES(hello_g PRIVATE ${GTK_INCLUDE_DIRS})
  #TARGET_LINK_DIRECTORIES(hello_g PRIVATE ${GTK_LIBRARY_DIRS})
  #TARGET_LINK_LIBRARIES(hello_g PRIVATE ${GTK_LIBRARIES} ws2_32 zmq)

ENDIF()

IF(MSYS OR MSVC)

  INSTALL(TARGETS trie_sum DESTINATION C:\\Programs\\bin)
  INSTALL(TARGETS tt DESTINATION C:\\Programs\\bin)
  INSTALL(TARGETS a DESTINATION C:\\Programs\\bin)
  INSTALL(TARGETS b DESTINATION C:\\Programs\\bin)
  INSTALL(TARGETS d DESTINATION C:\\Programs\\bin)

ELSE()

  INSTALL(TARGETS trie_sum DESTINATION ~/bin)
  INSTALL(TARGETS tt DESTINATION ~/bin)
  INSTALL(TARGETS a DESTINATION ~/bin)
  INSTALL(TARGETS b DESTINATION ~/bin)
  INSTALL(TARGETS d DESTINATION ~/bin)

ENDIF()
