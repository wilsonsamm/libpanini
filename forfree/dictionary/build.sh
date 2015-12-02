#!/bin/bash

make

# Build an English-Czech dictionary
echo Building the English to Czech dictionary
./dict-data "english" "(call noun singular)(lit /n/)" "czech" "(language (language czech)) (call noun singular nominative)" english.czech.dict
./dict-data "english" "(call verb infinitive)(lit /v/)" "czech" "(language (language czech)) (call verb infinitive) " english.czech.dict
./dictproc.py "english.czech.dict" "English to Czech Dictionary" > english.czech.dict.tex
pdflatex english.czech.dict.tex

echo Building the English thesaurus
./thes-data "english" "(call noun singular)(lit /n/)" english.thes
./thes-data "english" "(call verb infinitive)(lit /v/)" english.thes
./dictproc.py "english.thes" "English Thesaurus" > english.thes.tex
pdflatex english.thes.tex

echo Building the English to Japanese dictionary
./dict-data "english" "(call noun singular)(lit /n/)" "japanese" "(call noun)" english.japanese.dict
./dict-data "english" "(call verb infinitive)(lit /v/)" "japanese" "(call verb)" english.japanese.dict
./dictproc.py "english.japanese.dict" "English to Japanese Dictionary" > english.japanese.dict.tex
pdflatex english.japanese.dict.tex

echo Building the English to Ainu dictionary
./dict-data "english" "(call noun singular)(lit /n/)" "ainu" "(call noun)" english.ainu.dict
./dict-data "english" "(call adjective) (lit /adj/)" "ainu" "(call verb)" english.ainu.dict
./dict-data "english" "(call verb infinitive)(lit /v/)" "ainu" "(call verb)" english.ainu.dict
./dictproc.py "english.ainu.dict" "English to Ainu Dictionary" > english.ainu.dict.tex
pdflatex english.ainu.dict.tex
