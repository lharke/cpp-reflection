find_package(Catch2 REQUIRED)

add_executable(reflection.test reflection.cpp)
target_compile_features(reflection.test PRIVATE cxx_std_20)
target_link_libraries(reflection.test PRIVATE reflection Catch2::Catch2)

add_test(NAME reflection-test COMMAND reflection.test -o reflection.test.xml -r xml)
