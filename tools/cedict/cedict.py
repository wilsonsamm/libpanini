#!/usr/bin/python
import os
import codecs
import subprocess

def parserow(row, count):
	traditional = row.split(' ')[0] + "."
	simplified  = row.split(' ')[1] + "."
	pinyin      = row.split('[')[1].split(']')[0] + " ."
	
	for english in row.split('/')[1:-1]:
		if english[:2] != 'CL:':
			pr = subprocess.Popen(['./enzh', english + '.', pinyin, traditional, simplified])
			pr.wait()
		

cedict = os.environ['PANINI'] + "/resources/cedict/cedict_1_0_ts_utf-8_mdbg.txt"

with codecs.open(cedict, 'r', 'UTF-8') as cedictfile:
	count = 0
	for row in cedictfile:
		count = count + 1
		if row[0] != '#':
			parserow(row, count)
		if count % 1000 == 0:
			print "Current line number: ", str(count)
