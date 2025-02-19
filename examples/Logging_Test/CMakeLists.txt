cmake_minimum_required(VERSION 3.10)
project(LoggingTest)

include_directories(include)

add_executable(LoggingTest 
  LoggingTest.cpp
  include/circlog.cpp
)
