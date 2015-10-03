#!/bin/bash
if [ $# -gt 0 ]; then 
    PROFILE_DATA_FILE="$1"
else
    PROFILE_DATA_FILE='../bin/linux Clang/Release/profiledata.txt'
fi
awk -F, '{print $0 >> $1".txt"}' "${PROFILE_DATA_FILE}"
gnuplot 'plot.p'
