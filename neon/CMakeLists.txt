cmake_minimum_required(VERSION 3.10)
file(GLOB sources CONFIGURE_DEPENDS ./src/*.cc ./include/*.h ./main.cc)
add_executable(neon ${sources})
target_include_directories(neon PRIVATE ./include)
