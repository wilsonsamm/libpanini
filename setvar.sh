#!/bin/bash

# This is the root path where libpanini is. Later, Makefiles and other things
# use this to find the tools, and the library, etc. etc.
export PANINI=$(pwd)/

# These are the options that will be passed to gcc.
export CCOPTS="-ggdb -Wall -c"

# These are the options that will be passed to make.
export MAKEOPTS="-j4 --quiet --no-print-directory"
#export MAKEOPTS="-j4"
