cmake_minimum_required(VERSION 3.10)
project(HelloRatana)

add_executable(hello-ratana main.cpp)
install(TARGETS hello-ratana DESTINATION bin)
