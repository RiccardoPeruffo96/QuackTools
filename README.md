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

## How to use it

 * Linux systems: open the bash and follow the nexts commands:
   1. ``git clone https://github.com/RiccardoPeruffo96/QuackTools.git``
   2. ``mkdir build``
   3. ``cd build``
   4. ``CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..``
   5. ``make install``
     * ``make test`` (optional)
     * ``make doc`` (optional)
   6. ``cd ..``
   7. ``./bin/QuackTools``

Note: You can choose to use ``CC=clang CXX=clang++`` instead ``CC=gcc CXX=g++``

## Third party libreries

This code use:
 * wxWidgets to implements GUI (https://github.com/wxWidgets/wxWidgets);
 * Catch2 to implements tests (https://github.com/catchorg/Catch2);
 * Doxygen for documentation (https://github.com/doxygen/doxygen);
 * Cmake to generate Makefile (https://github.com/Kitware/CMake);
 * OpenCV to work with medias (https://github.com/opencv/opencv);
 * jsoncpp for json file management (https://github.com/open-source-parsers/jsoncpp).

## LICENSE

QuackTools is under GPL-3.0 license and it's free-to-use
