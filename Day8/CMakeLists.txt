# CMakeList.txt : CMake project for Day1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (Day8 "Day8.cpp")

# TODO: Add tests and install targets if needed.
target_compile_features(Day8 PRIVATE cxx_std_20)

