cmake_minimum_required(VERSION 3.22)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  Advent
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(common)
add_subdirectory(2023)
