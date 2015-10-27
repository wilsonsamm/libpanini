import codecs
s = {}
out = codecs.open('mandarin.csv', 'w', 'UTF-8')
with codecs.open('mandarin2.csv', 'r', 'UTF-8') as f:
	for line in f:
		fields = line.split(',')
		s[fields[0]] = fields[1]

with codecs.open('badsyllables.csv', 'r', 'UTF-8') as f:
	for line in f:
		fields = line.split(',')
		if fields[0] in s:
			out.write(fields[0] + ',' + s[fields[0]])
