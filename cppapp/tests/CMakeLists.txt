# In your tests/CMakeLists.txt
add_executable(my_tests my_test_file.cpp)
target_link_libraries(my_tests PRIVATE GTest::gtest_main)
gtest_discover_tests(my_tests)
