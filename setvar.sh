
# This file sets up some environment variables and eventually builds the whole
# system. The first section of this file sets some variables and these are
# arranged so that you can choose what to build and how.

# This is the root path where libpanini is. Later, Makefiles, compilers and
# other things use this to find the tools, header files, and the library, etc.
export PANINI=$(pwd)/

# These are the compilers to use for plain C, and C++
#export CCOMPILER="clang"
export CCOMPILER="gcc"
#export CXXCOMPILER="clang"
export CXXCOMPILER="g++"

# These are the options that will be passed to the compiler.
export CCOPTS="-ggdb -Wall -c"
#export CCOPTS="-fopenmp -Wall -c"

# These are the options that will be passed to make.
export MAKEOPTS="-j4 --quiet --no-print-directory"
#export MAKEOPTS="-j4"
