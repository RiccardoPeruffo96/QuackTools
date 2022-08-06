cmake_minimum_required(VERSION 3.15 FATAL_ERROR) #minimum version

#PROJ_NAME will be contains the name of the final runnable file
set(PROJ_NAME "QuackTools")

#PROJ_TEST_NAME will be contains the name of the test runnable file
set(PROJ_TEST_NAME "tests")

#I declare this is a C++ project
project(${PROJ_NAME} LANGUAGES CXX)

#set anothers flags
set(CMAKE_CXX_STANDARD 17) # force C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # no compiler extensions
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

#set CLANG-TIDY
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_CLANG_TIDY
  clang-tidy;
  -format-style='file';
  -header-filter=${CMAKE_CURRENT_SOURCE_DIR};
)

#set to enable multi-threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
#abort the process if you can't use a multithreads program
find_package(Threads REQUIRED)

#I declare the output release it's build/ folder
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#add the CMakeLists file to create the final program
add_subdirectory(source)

#add the CMakeLists file to create the program with unit test (catch2 library)
add_subdirectory(tests)

#enable run command
add_custom_target(run
  COMMAND
    ./../bin/QuackTools)

#enable doc command
add_custom_target(doc
  COMMAND
    doxygen ./Doxyfile
  WORKING_DIRECTORY
    ${CMAKE_SOURCE_DIR}
  COMMENT
    "Html doxygen docs created")

#enable docopen command
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

#enable test command
enable_testing()
add_test(NAME run_unit_test
    COMMAND ./tests/${PROJ_TEST_NAME}
  )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  #if i want to run in "Debug" mode
  #  then cmake already adds "-g -O0"

  #01: I print DEBUG
  message("DEBUG mode")

  #enable valgrind command
  add_custom_target(valgrind COMMAND valgrind ./../bin/QuackTools)
  #enable gdb
  add_custom_target(gdb COMMAND gdb ./../bin/QuackTools)

  #if compiler is gcc
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=memory")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-omit-frame-pointer")

  elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

    set(LLVM_USE_SANITIZER "Address;Memory;Undefined;Thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O1 -g")

  endif()

elseif (CMAKE_BUILD_TYPE STREQUAL "coverage" OR CODE_COVERAGE)

  message("CODE COVERAGE mode")

  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

    set(NAME_CCOV "Code Coverage")
    target_compile_options(${PROJECT_NAME} PRIVATE -lgcov --coverage -g -O0 "-lgcov --coverage -g -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lgcov --coverage -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgcov --coverage -g -O0")

    #enable gcovr command
    add_custom_target(cov COMMAND rm -r -f ../codecoverage COMMAND mkdir ../codecoverage COMMAND
        gcovr -r .. --html-title ${NAME_CCOV} --html --html-details -o ${CMAKE_SOURCE_DIR}/codecoverage/coverage.html --gcov-executable gcov)

  else()

    message(FATAL_ERROR "Code coverage requires GCC. Aborting.")

  endif()

else() #elseif (CMAKE_BUILD_TYPE STREQUAL "Release")

  #if we are in "Release" mode instead
  #  then cmake already adds "-O3 -DNDEBUG"

  #01: I print RELEASE
  message("RELEASE mode") # cmake already adds "-O3 -DNDEBUG"

endif()

#add_subdirectory(third_party)
