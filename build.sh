#!/bin/bash

build_and_run=true

sdl_special="`sdl2-config --cflags --libs` -lSDL2"
compiler_flags=""
warnings="-Wall -Wno-unused-variable"
#-Werror

if g++ -g $warnings $compiler_flags $sdl_special sdl_platform_layer.cpp -o ../build/platformer ;   
then
    if $build_and_run
       then 
	   cd ../build 
	   ./platformer ;
    fi
fi
