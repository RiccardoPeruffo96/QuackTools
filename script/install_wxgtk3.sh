#!/bin/bash

echo "-- start cloning wxWidgets..."
git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git $1/wxWidgets/

echo "-- moving to wxWidgets/buildgtk"
mkdir $1/wxWidgets/buildgtk
cd $1/wxWidgets/buildgtk

echo "-- calling cmake..."
cmake -G "Unix Makefiles" -DwxBUILD_TESTS=ALL wxBUILD_MONOLITHIC=ON wxBUILD_SHARED=OFF -DCMAKE_BUILD_TYPE=Release ..

echo "-- compiling wxWidgets"
make

echo "-- install library"
sudo make install
