# Google Test
# include(GoogleTest)
# fetchcontent_makeavailable(googletest)

# INCLUDE(FetchContent)
# FETCHCONTENT_DECLARE(
#   googletest
#   URL https://github.com/google/googletest/archive/release-1.11.0.zip
# )
# FETCHCONTENT_MAKEAVAILABLE(googletest)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# include(FetchContent)
# fetchcontent_declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG release-1.12.1
# )

add_executable(codetest codetest.cpp)
add_executable(hello_test hello_test.cpp)

target_link_libraries(codetest GTest::gtest_main)
target_link_libraries(hello_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(codetest)
gtest_discover_tests(hello_test)

add_test(NAME codetest COMMAND codetest)
add_test(NAME hello_test COMMAND hello_test)

