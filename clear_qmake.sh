#!/bin/sh

####################################################
# Removes remaining qmake files from qt5ct package #
####################################################

make distclean
find . -type f -name *.qm -delete
find . -type d -name ".build" | xargs rm -rf
