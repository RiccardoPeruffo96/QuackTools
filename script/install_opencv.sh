#!/bin/bash

echo "-- start cloning opencv..."
wget -O $1/opencv.zip https://github.com/opencv/opencv/archive/master.zip
unzip $1/opencv.zip -d $1
mv $1/opencv-master $1/opencv

echo "-- moving to opencv/build"
mkdir $1/opencv/build
cd $1/opencv/build

echo "-- calling cmake..."
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DOPENCV_DOWNLOAD_PATH=/tmp/opencv-cache ..

echo "-- compiling jsoncpp"
make

echo "-- install library"
sudo make install