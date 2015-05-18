#!/bin/bash

for i in {axe banana beach bird boat bone book bread car cat cave condom colour\
		  duck eraser flavour fruit heaven house oyster rabbit road room thing \
		  tree village whale world worm\
		  arm body bone centre eye hand \
		  father friend mother person soldier uncle visitor \
		  august april day december east february march january july june north\
		  november september south summer time week west winter year \
		  }
do
	$PANINI/tools/wn/wn-bootstrap noun $i
done
