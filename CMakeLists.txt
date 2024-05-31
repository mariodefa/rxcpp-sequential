cmake_minimum_required(VERSION 3.14)
project(myproject)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add vcpkg.cmake
set(CMAKE_TOOLCHAIN_FILE "/vcpkg/scripts/buildsystems/vcpkg.cmake")

# add rxcpp external library
find_package(rxcpp CONFIG REQUIRED)

# Example4 
add_executable(Example4 example4.cpp)
target_link_libraries(Example4 PRIVATE rxcpp)
# add rxcpp external headers
target_include_directories(Example4 PRIVATE /vcpkg/installed/x64-linux/include)