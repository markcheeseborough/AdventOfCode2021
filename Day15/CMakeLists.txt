# CMakeList.txt : CMake project for Day1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Day15 "Day15.cpp")

# TODO: Add tests and install targets if needed.
target_compile_features(Day15 PRIVATE cxx_std_20)

