cmake_minimum_required(VERSION 3.9)
project(Trusted_hardware)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include/jsoncpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
add_subdirectory(HW)
include_directories(HW/include)

include_directories(include)
link_directories(lib)
add_library(hardware_lib
    src/Merkle_Hash.cpp
    src/protocol.cpp
    src/network.cpp
    src/tool.cpp
    )
target_link_libraries(hardware_lib libjson.so hardware)
add_executable(main src/main.cpp)

target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT} hardware_lib)
add_executable(file_deal src/_file_deal.cpp)
add_executable(test_ex test/test.cpp)
target_link_libraries(test_ex hardware hardware_lib)

