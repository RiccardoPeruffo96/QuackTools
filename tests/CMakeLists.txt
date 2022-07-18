cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

set(TEST_PROJ_NAME tests)

set(ACTUAL_DIR_TEST ${CMAKE_SOURCE_DIR}/tests)
file(GLOB_RECURSE local_test *.cpp *.hpp ../source/src/*.cpp)
list(FILTER local_test EXCLUDE REGEX ".*gui\\.cpp$")
list(FILTER local_test EXCLUDE REGEX ".*log_operations\\.cpp$")
list(FILTER local_test EXCLUDE REGEX ".*file_operations\\.cpp$")

include_directories(${ACTUAL_DIR_TEST}) #tests root
include_directories(${ACTUAL_DIR_TEST}/../include) #public interface

add_executable(${TEST_PROJ_NAME} ${local_test})

#set threads flag - how show in cmake docs
set(THREADS_PREFER_PTHREAD_FLAG ON)

#search about required libraries
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)

#include libraries inside the program
include_directories(${OpenCV_INCLUDE_DIRS})

#link the libraries
target_link_libraries(${TEST_PROJ_NAME} PRIVATE Threads::Threads ${OpenCV_LIBS})
