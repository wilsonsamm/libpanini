def pronouns(features):
	for case in features['PronominalCoreCases']:
		print '(df pronoun'
		if case != 'neutral':
			print '\t(rection (case ' + case + '))'
		print '\t(flags ' + case + ')'
		print '\t(open pronoun)'
		print '\t(call guess-syllables)'
		print '\t(add-record-to-df pronoun)'
		if case != 'neutral':
			print '\t(add-flags-to-df pronoun ' + case + ')'
		print '\t(add-ns-to-df pronoun seme)'
		print '\t(add-ns-to-df pronoun language)'
		print '\t(add-ns-to-df pronoun rection)'
		print '\t(add-tag-to-df pronoun))'

def nominal_case_morpheme(case):
	if case != 'neutral':
		print '(df NominalCaseSuffix'
		print '\t(open NominalCaseSuffix)'
		print '\t(call guess-syllables)'
		print '\t(add-record-to-df NominalCaseSuffix)'
		print '\t(add-flags-to-df NominalCaseSuffix ' + case + ')'
		print '\t(add-ns-to-df NominalCaseSuffix language)'
		print '\t(add-ns-to-df NominalCaseSuffix rection)'
		print '\t(add-tag-to-df NominalCaseSuffix))'


def nounstems(features):
	print '(df nounstem'
	print '\t(open nounstem)'
	print '\t(call guess-syllables)'
	print '\t(add-record-to-df nounstem)'
	print '\t(add-ns-to-df nounstem seme)'
	print '\t(add-ns-to-df nounstem language)'
	print '\t(add-tag-to-df nounstem))'

def argument(argtype, case):
	print "; call argument('" + argtype + "','" + case + ')'
	if argtype == 'a':
		df = '(df ArgA'
	if argtype == 'o':
		df = '(df ArgO'
	if argtype == 's':
		df = '(df ArgS'
	casus = df + ' (into (theta ' + case + ') '
	prondf = casus + 'call pronoun ' + case + '))'
	noundf = casus + 'call nounphrase ' + case + '))'
	print prondf
	print noundf

def nounphrase(case):
	print '(df nounphrase'
	print '\t(flags ' + case + ')'
	if case != 'neutral':
		print '\t(rection (case ' + case + '))'
	print '\t(call noun))'


def generate_nouns(features):
	pronouns(features)
	nounstems(features)
	if 'NominalCoreCases' in features:
		for case in features['NominalCoreCases']:
			nominal_case_morpheme(str(case))
			nounphrase(str(case))
			argument('a', str(case))
			argument('s', str(case))
			argument('o', str(case))
	print '(df noun'
	print '\t(call nounstem)'
	print '\t(call NominalCaseSuffix))'
