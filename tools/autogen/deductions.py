"""
The purpose of these functions is to make any deductions about the language from
the data we've collected. For example, we may have found out that a language has
an SVO word order, and this will make some (quite trivial) deductions about what
that means for the placement of the subject in the syntax tree.
All of this will get collected in the dictionary referred to by the name
"features"

"""

def CaseAlignments(alignments, cases):
	if '2 Nominative - accusative (standard)' in alignments:
		cases.append('nominative')
		cases.append('accusative')
	elif '3 Nominative - accusative (marked nominative)' in alignments:
		cases.append('nominative')
		cases.append('accusative')
	elif '4 Ergative - absolutive' in alignments:
		cases.append('ergative')
		cases.append('absolutive' in alignments)
	elif '5 Tripartite' in alignments:
		cases.append('ergaive')
		cases.append('absolutive')
		cases.append('accusative')

def deductions(features):
	if 'NominalCoreCases' not in features:
		features['NominalCoreCases'] = []
	if 'PronominalCoreCases' not in features:
		features['PronominalCoreCases'] = []
	if 'wals_98A' in features:
		CaseAlignments(features['wals_98A'], features['NominalCoreCases'])
	if 'wals_99A' in features:
		CaseAlignments(features['wals_99A'], features['PronominalCoreCases'])
	if not features['NominalCoreCases']:
		features['NominalCoreCases'].append('neutral')
	if not features['PronominalCoreCases']:
		features['PronominalCoreCases'].append('neutral')
	if 'WordOrders' not in features:
		features['WordOrders'] = []
	if 'wals_81A' in features:
		if '1 SOV' in features['wals_81A']:
			features['WordOrders'].append('SOV')
		if '2 SVO' in features['wals_81A']:
			features['WordOrders'].append('SVO')
		if '3 VSO' in features['wals_81A']:
			features['WordOrders'].append('VSO')
	if 'wals_81B' in features:
		if '1 SOV or SVO' in features['wals_81B']:
			features['WordOrders'].append('SOV')
			features['WordOrders'].append('SVO')
		if '2 VSO or VOS' in features['wals_81B']:
			features['WordOrders'].append('VSO')
			features['WordOrders'].append('VOS')
		if '3 SVO or VSO' in features['wals_81B']:
			features['WordOrders'].append('SVO')
			features['WordOrders'].append('VSO')
		if '4 SVO or VOS' in features['wals_81B']:
			features['WordOrders'].append('SVO')
			features['WordOrders'].append('VOS')
		if '5 SOV or OVS' in features['wals_81B']:
			features['WordOrders'].append('SOV')
			features['WordOrders'].append('OVS')
	if 'SubjectPlacement' not in features:
		features['SubjectPlacement'] = []
	if 'SOV' in features['WordOrders']:
		features['SubjectPlacement'].append('high')
	if 'SVO' in features['WordOrders']:
		features['SubjectPlacement'].append('high')
	if 'VSO' in features['WordOrders']:
		features['SubjectPlacement'].append('low')
