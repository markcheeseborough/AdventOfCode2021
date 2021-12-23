# CMakeList.txt : CMake project for Day1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Day7 "Day7.cpp")

# TODO: Add tests and install targets if needed.
target_compile_features(Day7 PRIVATE cxx_std_20)

