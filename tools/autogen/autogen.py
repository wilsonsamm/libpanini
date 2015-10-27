import sys
import os
import csv

from generate import generate_src
from deductions import deductions
db = {}

db['target'] = sys.argv[1]

with open('targets.csv') as targets:
	for line in targets:
		if db['target'] == line[:-1].split(',')[0]:
			db['isocodes'] = line[:-1].split(',')[1]

with open(os.environ['PANINI']+'/resources/wals/language.csv', 'rb') as wals:
	reader = csv.reader(wals, delimiter = ',', quotechar='"')
	headers = next(reader)
	
	for line in reader:
		if line[1] in db['isocodes'] and line[1] != '':
			for header in headers:
				feature = 'wals_' + header.split(' ')[0]
				if line[headers.index(header)] != '':
					if feature not in db:
						db[feature] = []
					if line[headers.index(header)] not in db[feature]:
						db[feature].append(line[headers.index(header)])

deductions(db)
generate_src(db)
