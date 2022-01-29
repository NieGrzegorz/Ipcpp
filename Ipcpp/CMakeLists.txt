# CMakeList.txt : CMake project for Ipcpp.lib, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(Threads)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source to this project's executable.
add_executable (Ipcpp "TestApp.cpp" "ipcsocket.hpp")
target_link_libraries(Ipcpp Threads::Threads)

# TODO: Add tests and install targets if needed.
