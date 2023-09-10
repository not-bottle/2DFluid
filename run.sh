#!/bin/bash

if g++ main.cpp -o main; then
    cp image.ppm prev.ppm
    ./main > image.ppm
    gimp image.ppm
fi