#!/bin/bash
echo "Clean configuration..."
make distclean
echo "Load configuration..."
make jz2440_defconfig
echo "Make u-boot..."
make all -j4
