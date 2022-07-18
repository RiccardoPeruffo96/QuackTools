cmake_minimum_required(VERSION 3.9 FATAL_ERROR) #minimum version

#PROJ_NAME will be contains the name of the final runnable file
set(PROJ_NAME "QuackTools")

#PROJ_TEST_NAME will be contains the name of the test runnable file
set(PROJ_TEST_NAME "tests")
set(PROJ_TEST_DIR "bin/test/")

#I declare this is a C++ project
project(${PROJ_NAME} LANGUAGES CXX)

#set anothers flags
set(CMAKE_CXX_STANDARD 17) # force C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # no compiler extensions
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

#set to enable multi-threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
#abort the process if you can't use a multithreads program
find_package(Threads REQUIRED)

#I declare the output release it's build/ folder
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#add the CMakeLists file to create the final program
add_subdirectory(source)

#add the CMakeLists file to create the program with unit test (catch2 library)
#add_subdirectory(tests)

#enable doc command
add_custom_target(doc
  COMMAND
    doxygen ./Doxyfile
  WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
  COMMENT
    "Html doxygen docs created")

#enable doc command
add_custom_target(docopen
  COMMAND
    doxygen ./Doxyfile
  COMMAND
   xdg-open doc/html/index.html
  WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
  COMMENT
    "Html doxygen docs created")

#${CMAKE_COMMAND} <- relative root dir
#enable install command
install(
  TARGETS
    ${PROJ_NAME}
  DESTINATION
    ${CMAKE_SOURCE_DIR}/bin)
#install(
#  TARGETS
#    ${PROJ_TEST_NAME}
#  DESTINATION
#    ${CMAKE_SOURCE_DIR}/${PROJ_TEST_DIR})

#enable test command
enable_testing()
add_test(NAME run_unit_test
    COMMAND ./../${PROJ_TEST_DIR}/${PROJ_TEST_NAME}
  )

#if i want to run in "Debug" mode
#  then cmake already adds "-g -O0"
if (CMAKE_BUILD_TYPE STREQUAL "Debug")

  #01: I print DEBUG
  message("DEBUG mode")

  #if compiler is gcc
  if (CMAKE_COMPILER_IS_GNUCXX)

    #then I add the flag "-ggdb3"
    target_compile_options(${PROJECT_NAME} PUBLIC "-ggdb3")

  endif()

#if we are in "Release" mode instead
#  then cmake already adds "-O3 -DNDEBUG"
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")

  #01: I print RELEASE
  message("RELEASE mode") # cmake already adds "-O3 -DNDEBUG"

endif()

#add_subdirectory(third_party)
