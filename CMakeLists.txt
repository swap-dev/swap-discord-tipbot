cmake_minimum_required(VERSION 3.10)
project (swap-discord-bot)
set (CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE sources "src/*.cpp")
add_subdirectory(extern/sleepy-discord)
add_subdirectory(extern/curlcpp)
add_subdirectory(extern/redox)
include_directories(extern/curlcpp/include)
include_directories(extern/redox/include)

set(EXECUTABLE_OUTPUT_PATH "bin")
set(LIBRARY_OUTPUT_PATH "bin")

add_executable(swap-discord-bot ${sources})
target_link_libraries(swap-discord-bot sleepy-discord curlcpp redox ev hiredis)
