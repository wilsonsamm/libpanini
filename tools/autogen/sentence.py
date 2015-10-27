"""
The purpose of this file is to generate source code to define the sentence-level
syntax as defined by the values in the pre-prepared dictionary called "features".
"""

def SubjectPlacementHigh():
	print '(df main (call s))'
	print '(df s (call ArgA) (call auxp))'
	print '(df auxp (call aux) (call vp infinite))'


def SubjectPlacementLow():
	print '(df main (call auxp))'
	print '(df s (call auxp) (call ArgA))'
	print '(df auxp (call aux) (call s))'


def VerbFinal():
	print '(for i (infinitive finite)'
	print '\t(df vp'
	print '\t\t(flags .i)'
	print '\t\t(call ArgO)'
	print '\t\t(call verb .i)))'


def VerbInitial():
	print '(for i (infinitive finite)'
	print '\t(df vp'
	print '\t\t(flags .i)'
	print '\t\t(call verb .i)'
	print '\t\t(call ArgO)))'


def generate_sentence(features):
	if 'high' in features['SubjectPlacement']:
		SubjectPlacementHigh()
	if 'low' in features['SubjectPlacement']:
		SubjectPlacementLow()
	if 'VPHead' not in features:
		print '; PROBLEM: VPHead feature not defined'
	else:
		if 'final' in features['VPHead']:
			VerbFinal()
		if 'initial' in features['VPHead']:
			VerbInitial()
