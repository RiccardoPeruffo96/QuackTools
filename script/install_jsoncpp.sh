#!/bin/bash

echo "-- start cloning jsoncpp..."
git clone --recurse-submodules https://github.com/open-source-parsers/jsoncpp.git $1/jsoncpp/

echo "-- moving to jsoncpp/build"
mkdir $1/jsoncpp/build
cd $1/jsoncpp/build

echo "-- calling cmake..."
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..

echo "-- compiling jsoncpp"
make

echo "-- install library"
sudo make install
