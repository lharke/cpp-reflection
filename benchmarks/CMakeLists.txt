find_package(benchmark REQUIRED)

add_executable(reflection.benchmark reflection.cpp)
target_compile_features(reflection.benchmark PRIVATE cxx_std_20)
target_link_libraries(reflection.benchmark PRIVATE reflection benchmark::benchmark)
