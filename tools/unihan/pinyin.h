
#define PINYIN_DIA 0
#define PINYIN_NUM 1

#define INITIALS 23
#define CODAS 165
char * convert(char * pinyin, int from, int to);
/* These arrays define the various orthographies of mandarin chinese.
 * Those #defines up there are array subscripts that define where to look in the
 * array for your orthographic representation. */


static char * initials[INITIALS][2] = {

	{"b", "b"},
	{"p", "p"},
	{"m", "m"},
	{"f", "f"},
	
	{"d", "d"},
	{"t", "t"},
	{"n", "n"},
	{"l", "l"},
	
	{"g", "g"},
	{"k", "k"},
	{"h", "h"},
	
	{"j", "j"},
	{"q", "q"},
	{"x", "x"},
	
	{"zh", "zh"},
	{"ch", "ch"},
	{"sh", "sh"},
	{"r",  "r"},
	
	{"z", "z"},
	{"c", "c"},
	{"s", "s"},
	{"y", "y"},
	{"w", "w"}
};

static char * codas[CODAS][2] = {
	{"uāng", "uang1"},  {"uáng", "uang2"},   {"uǎng", "uang3"},   {"uàng", "uang4"},   {"uang",  "uang5"},
	{"ūn",   "un1"}, {"ún",   "un2"}, {"ǔn",   "un3"}, {"ùn",   "un4"}, {"un", "un5"},

	{"uī",   "ui1"}, {"uí",   "ui2"}, {"uǐ",   "ui3"}, {"uì",   "ui4"}, {"ui", "i5"},
	{"uān", "uan1"},  {"uán", "uan2"},   {"uǎn", "uan3"},   {"uàn", "uan4"},   {"uan",  "uan5"},
	{"uāi", "uai1"},  {"uái", "uai2"},   {"uǎi", "uai3"},   {"uài", "uai4"},   {"uai",  "uai5"},
	{"āi", "ai1"},  {"ái", "ai2"},   {"ǎi", "ai3"},   {"ài", "ai4"},   {"ai",  "ai5"},
	{"āo", "ao1"}, {"áo", "ao2"}, {"ǎo", "ao3"},{"ào", "ao4"}, {"ao", "ao5"},
	{"āng", "ang1"},{"áng", "ang2"}, {"ǎng", "ang3"},{"àng", "ang4"}, {"ang", "ang5"},
	{"ān", "an1"}, {"án", "an2"}, {"ǎn", "an3"},{"àn", "an4"}, {"an", "an5"},
	
	{"uā",   "ua1"}, {"uá",   "ua2"}, {"uǎ",   "ua3"}, {"uà",   "ua4"}, {"ua",   "ua5"},
	{"ā",   "a1"}, {"á",   "a2"}, {"ǎ",   "a3"}, {"à",   "a4"}, {"a",   "a5"},

	{"ēng",   "eng1"}, {"éng",   "eng2"}, {"ěng",   "eng3"}, {"èng",   "eng4"},{"eng",   "eng5"},
	{"ēr",   "er1"}, {"ér",   "er2"}, {"ěr",   "er3"}, {"èr",   "er4"},{"er",   "er5"},
	{"ēn",   "en1"}, {"én",   "en2"}, {"ěn",   "en3"}, {"èn",   "en4"},{"en",   "en5"},
	{"ēi",   "ei1"}, {"éi",   "ei2"}, {"ěi",   "ei3"}, {"èi",   "ei4"},{"ei",   "ei5"},
	{"uē",   "ue1"}, {"ué",   "ue2"}, {"uě",   "ue3"}, {"uè",   "ue4"},{"ue",   "ue5"},
	{"ē",   "e1"}, {"é",   "e2"}, {"ě",   "e3"}, {"è",   "e4"},{"e",   "e5"},
	{"iē",   "ie1"}, {"ié",   "ie2"}, {"iě",   "ie3"}, {"iè",   "ie4"},{"ie",   "e5"},
	
	{"iāng","iang1"},	{"iáng","iang2"},	{"iǎng","iang3"},	{"iàng","iang5"},	{"iang","iang5"},
	{"iān","ian1"},		{"ián","ian2"},		{"iǎn","ian3"}, 	{"iàn","ian4"},	{"ian","ian5"},
	{"iāo", "ia1o"},	{"iáo","iao2"},		{"iǎo","iao3"}, 	{"iào","iao4"},	{"iao","iao5"},
	{"iā", "ia1"},		{"iá","ia2"},		{"iǎ","ia3"}, 	{"ià","ia4"},	{"ia","ia5"},
	{"īng",   "ing1"}, {"íng",   "ing2"}, {"ǐng",   "ing3"}, {"ìng",   "ing4"}, {"ing", "ing5"},
	{"īn",   "in1"}, {"ín",   "in2"}, {"ǐn",   "in3"}, {"ìn",   "in4"}, {"in", "in5"},

	{"ōu", "ou1"}, {"óu", "ou2"}, {"ǒu", "ou2"}, {"òu", "ou4"}, {"ou", "ou5"},
	{"uō", "uo1"}, {"uó", "uo2"}, {"uǒ", "uo2"}, {"uò", "uo4"}, {"uo", "uo5"},
	{"iū",   "iu1"}, {"iú",   "iu2"}, {"iǔ",   "iu3"}, {"iù",   "iu4"}, {"iu", "iu5"},
	{"üè", "u:e2"},
	{"ū",   "u1"}, {"ú",   "u2"}, {"ǔ",   "u3"}, {"ù",   "u4"}, {"u", "u5"},
	{"ǘ", "u:2"}, {"ǜ", "u:4"}, {"ǚ", "u:3"}, {"ü", "u:5"},
	{"iōng", "iong1"}, {"ióng", "iong2"}, {"iǒng", "iong2"}, {"iòng", "iong4"}, {"iong", "iong5"},
	{"ōng", "ong1"}, {"óng", "ong2"}, {"ǒng", "ong2"}, {"òng", "ong4"}, {"ong", "ong5"},
	{"ō", "o1"}, {"ó", "o2"}, {"ǒ", "o2"}, {"ò", "o4"}, {"o", "o5"},
	{"ī",   "i1"}, {"í",   "i2"}, {"ǐ",   "i3"}, {"ì",   "i4"}, {"i", "i5"}

};
