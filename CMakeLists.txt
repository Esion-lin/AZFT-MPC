cmake_minimum_required(VERSION 3.5)
project(Trusted_hardware)


include_directories(include/jsoncpp)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_subdirectory(HW)
include_directories(HW/include)

include_directories(include)
link_directories(lib)
add_library(hardware_lib
	src/component.cpp
    src/protocol.cpp
    src/network.cpp
    src/keccak.cpp
    src/tool.cpp
    )
target_link_libraries(hardware_lib libjson.so hardware)
add_executable(main src/main.cpp)

target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT} hardware_lib)
add_executable(file_deal src/_file_deal.cpp)
add_executable(code_gen test/gen_code.cpp)
#add_executable(test_ex test/test.cpp)
#add_executable(test_cal test/cal_plain.cpp)
#target_link_libraries(test_ex hardware hardware_lib)
#target_link_libraries(test_cal hardware hardware_lib)
target_link_libraries(code_gen hardware hardware_lib)
