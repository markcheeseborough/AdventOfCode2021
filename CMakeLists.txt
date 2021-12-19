# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("AdventOfCode2021")

# Include sub-projects.
add_subdirectory ("Day1")
add_subdirectory ("Day2")
add_subdirectory ("Day3")
