cmake_minimum_required(VERSION 3.9)

#project(app1 cxx)
project(app1)

file(WRITE "src/main.cc" "#include \"a.h\"\n#include \"b.h\"\nint main()\n{\na();\nb();\nreturn 0;\n}")
file(WRITE "../lib/liba/Inc/a.h" "void a();")
file(WRITE "../lib/liba/src/a.cc" "#include \"c.h\"\nvoid a()\n{\nc();\n}")
file(WRITE "../lib/libb/Inc/b.h" "void b();")
file(WRITE "../lib/libb/src/b.cc" "void b() {}")
file(WRITE "../lib/libc/Inc/c.h" "void c();")
file(WRITE "../lib/libc/src/c.cc" "#include \"b.h\"\nvoid c()\n{\nb();\n}")

include_directories(
    ../lib/liba/Inc
    ../lib/libb/Inc
)

add_subdirectory(../lib/liba liba)
add_subdirectory(../lib/libb libb)

add_executable(app1 src/main.cc)

target_link_libraries(app1 liba libb)

