cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

#add_subdirectory(aes256)
#add_subdirectory(sha256)
#add_subdirectory(diffie_hellman)

set(TEST_PROJ_NAME tests)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")

  #01: I print DEBUG
  message("DEBUG mode")

  #if compiler is gcc
  if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))

    #then I add the flag "-ggdb3"
    target_compile_options(${PROJECT_NAME} PUBLIC "-ggdb3")

  endif()

#if we are in "Release" mode instead
#  then cmake already adds "-O3 -DNDEBUG"
#elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
else()

  #01: I print RELEASE
  message("RELEASE mode") # cmake already adds "-O3 -DNDEBUG"

endif()

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
