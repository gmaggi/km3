#!/bin/zsh

if [ -z "$JPP_LIB" ]; then
    echo "ERROR: You have to source JPP first"
else
    echo "Setting variables for KM3 H5CPPConverter"    
    if [ $# -eq 1 ]; then
	export H5_CPP_CONVERTER_DIR=$1
    else 
	export H5_CPP_CONVERTER_DIR=$(pwd)/
    fi
    echo "adding h5 concatenate scripts to JPP_BIN"
    chmod +x ${H5_CPP_CONVERTER_DIR}/h5Concatenate.py
    rm -rf $JPP_BIN/h5Concatenate.py
    ln -s ${H5_CPP_CONVERTER_DIR}/h5Concatenate.py $JPP_BIN/h5Concatenate.py
fi

