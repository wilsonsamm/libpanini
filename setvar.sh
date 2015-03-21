
# This file sets up some environment variables and eventually builds the whole
# system. The first section of this file sets some variables and these are
# arranged so that you can choose what to build and how.

# This is the root path where libpanini is. Later, Makefiles, compilers and
# other things use this to find the tools, header files, and the library, etc.
export PANINI=$(pwd)/

# These are the compilers to use for plain C, and C++
#export CCOMPILER="clang"
export CCOMPILER="gcc"
export CXXCOMPILER="clang"
#export CXXCOMPILER="g++"

# These are the options that will be passed to the compiler.
export CCOPTS="-ggdb -Wall -c"

# These are the options that will be passed to make.
export MAKEOPTS="-j4 --quiet --no-print-directory"
#export MAKEOPTS="-j4"

# Select which language module you want to build. If you don't care about a
# particular language module, you can set the variable to zero here.
export PANINI_AI=1	# Ainu
export PANINI_AL=1	# Algonquian (includes Cree, Ojibwe, etc.)
export PANINI_CZ=1	# Czech
export PANINI_JP=1	# Japanese
export PANINI_AZ=1	# Nahuatl (aka. Aztec)
export PANINI_QU=1	# Quenya
export PANINI_SP=1	# Spanish
export PANINI_SW=1	# Swahili

# If you have EDICT and KANJIDIC installed, then ekan can use them to learn some
# important Japanese phrases, words and symbols. Uncomment this following line
# if you do want to use this feature. If you choose not to do this, the Japanese
# module may still be built, but without much vocabulary and without any kanji
# or kana (basically useless). 
export PANINI_EKAN="ekan.out"

# If you have various bibles installed on your computer, then PP can use them to
# learn an arbitrary language, filling in any gaps in the dictionaries and
# working out the syntax of that language.
# Uncomment these lines if you would like to use this facility.
# PP_SWORD may be set to 0 for no or 1 for yes.
export PP_SWORD=1
# Uncomment some of these, to use a bible for a particular language.
# For each language, enable at most one bible.

#export PP_SWORD_EN="BBE"		# To use the Basic English Bible
export PP_SWORD_EN="KJV"		# To use the King James Version
export PP_SWORD_CZ="CzeBKR" 	# To use the Czech Bible Kralicka
#export PP_SWORD_CZ="CzeKMS" 	# To use the Czech Překlad KMS Nová smlouva
#export PP_SWORD_LA="Vulgata" 	# To use the Latin Vulgate
export PP_SWORD_SP="SpaRV"		# To use the Spanish Reina-Valera
