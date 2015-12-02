import codecs
s = {}

with codecs.open('mandarin.csv', 'r', 'UTF-8') as f:
	for line in f:
		fields = line.split(',')
		if fields[0] not in s:
			s[fields[0]] = fields[1]

for f in s:
	print s[f][0] + "," + s[f][1]
