<a id="top"></a>
![QuackTools logo](data/media/duck.png)

# QuackTools

Some security features for everyone!

## What is QuackTools?

Is a program that allows to use easily a large range of security protocols.
With this program you are able:
 * to hide your files inside other media files;
 * to encrypt and decrypt with strong algorithm (CRT-AES256);
 * to generate private keys or create a key with Diffie-Hellman for data exchange;
 * to use encode and decode protocol with base64.

This program is tested with Linux but should be work fine with others systems.

This program follows c++17 standards.

## How to compile the source

 * Linux systems: open the bash and follow the nexts commands (minimum requirement ``build essential`` and ``cmake`` already installed, others dipendencies will be installed automatically)
   1. ``git clone https://github.com/RiccardoPeruffo96/QuackTools.git``
   2. ``cd QuackTools``
   3. ``mkdir build``
   4. ``cd build``
   5. ``sudo CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TYPE=Release ..``
       * or ``sudo CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Release ..``
   6. ``make install``
       * ``make test`` (optional)
       * ``make doc`` (optional)
   7. ``make run``

Note: if clang-tidy is not defined, .clang-tidy it will be ignored.

## Different compile options

 * To enable debug: ``CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Debug ..`` or ``CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TYPE=Debug ..``
   * after that the follow commands are enabled (you need to run ``make install``): ``make debug`` (you need to have gdb installed if CC=gcc CXX=g++ or lldb installed if CC=clang CXX=clang++) and ``make valgrind`` (you need to have valgrind installed);
 * To enable code coverage: ``CC=gcc CXX=g++ cmake -DCMAKE_BUILD_TYPE=coverage ..``
   * after that the follow commands are enabled (you need to run ``make install`` then ``make run``): ``make cov`` to generate the code coverage at ``/codecoverage/coverage.html`` (you need to have gcov and gcovr installed)

## Third party libreries

This code use:
 * wxWidgets to implements GUI (https://github.com/wxWidgets/wxWidgets);
 * Catch2 to implements tests (https://github.com/catchorg/Catch2);
   * like the follow fedora patch (https://src.fedoraproject.org/rpms/catch1/blob/rawhide/f/catch1-sigstksz.patch), i'll change the var SIGSTKSZ with the const value 32768 (32 x 1024) because a well known issue (https://github.com/mapbox/variant/issues/183)
 * Doxygen for documentation (https://github.com/doxygen/doxygen);
 * Cmake to generate Makefile (https://github.com/Kitware/CMake);
 * OpenCV to work with medias (https://github.com/opencv/opencv);
 * jsoncpp for json file management (https://github.com/open-source-parsers/jsoncpp).

## LICENSE

QuackTools is under GPL-3.0 license and it's free-to-use
