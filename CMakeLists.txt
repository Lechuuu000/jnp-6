cmake_minimum_required(VERSION 3.17)
project(jnp_6)

set(CMAKE_CXX_STANDARD 17)

add_executable(example computer.h ooasm.h task/ooasm_example.cc)
add_executable(test_grzeska computer.h ooasm.h test_grzesiek.cpp)
add_executable(test_rafala computer.h ooasm.h test_rafal.cpp)
add_executable(test_krzyska computer.h ooasm.h test_krzysiek.cpp)
add_executable(my_test computer.h ooasm.h test.cpp)
