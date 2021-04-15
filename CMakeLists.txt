cmake_minimum_required (VERSION 3.2.0)
project (ccol)

set(Boost_NO_SYSTEM_PATHS ON)

find_package(Boost 1.75.0 REQUIRED COMPONENTS json regex program_options)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(ccol ccol.cpp)

target_link_libraries(ccol ${Boost_LIBRARIES})
