#!/usr/bin/python

import codecs
import sys

with codecs.open(sys.argv[1], 'r', 'UTF-8') as dictfile:
	count = 0
	currentdict = {}
	for row in dictfile:
		fields = row.split('/')
		headword = fields[0].strip()
		pos = fields[1].strip()
		translation = fields[2].strip()

		if headword not in currentdict:
			currentdict[headword] = {}

		entry = currentdict[headword]

		if pos not in entry:
			entry[pos] = []

		if translation not in entry[pos]:
			entry[pos].append(translation)

print "\\documentclass{article}"
print "\\usepackage[utf8]{inputenc}"
print "\\usepackage{lmodern}"
print "\\usepackage{textcomp}"
print "\\usepackage{multicol}"
print "\\begin{document}"
print "\\title{" + sys.argv[2] + "}"
print "\\maketitle"
print "\setlength{\parskip}{3pt plus 1pt minus 1pt}"
print "\\begin{multicols}{2}"

for lemma in sorted(currentdict):
	for e in sorted(currentdict[lemma]):
		print u"\\textbf{"+lemma+"} \\textit{"+e+"}"
		for tr in currentdict[lemma][e]:
			print ", " + tr.encode('utf8')
		print "\n\n"

print "\\end{multicols}"
print "\\end{document}"

#print currentdict
