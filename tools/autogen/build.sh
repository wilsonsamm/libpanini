#!/bin/bash

for target in english ainu algonquian czech inuit chinese swahili saami scandi latvian
do
	echo Autogen is creating autogen-$target.panini
	python autogen.py $target > autogen-$target.panini
done
