
add_library(common OBJECT strings.cpp)

target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
