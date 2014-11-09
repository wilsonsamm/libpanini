#include <stdio.h>
static char * katakana[] = {
	"ア", "イ", "ウ", "エ", "オ",
	"カ", "キ", "ク", "ケ", "コ",
	"ガ", "ギ", "グ", "ゲ", "ゴ",
	"サ", "シ", "ス", "セ", "ソ",
	"ザ", "ジ", "ズ", "ゼ", "ゾ",
	"タ", "チ", "ツ", "テ", "ト",
	"ダ", "ヂ", "ヅ", "デ", "ド",
	"ハ", "ヒ", "フ", "ヘ", "ホ",
	"バ", "ビ", "ブ", "ベ", "ボ",
	"パ", "ピ", "プ", "ペ", "ポ",
	"マ", "ミ", "ム", "メ", "モ",
	"ナ", "ニ", "ヌ", "ネ", "ノ",
	"ラ", "リ", "ル", "レ", "ロ",
	"ヤ",      "ユ",       "ヨ",
	"ワ",            "ヲ", "ン"
	""
};

static char * hiragana[] = {
	"あ", "い", "う", "え", "お",
	"か", "き", "く", "け", "こ",
	"が", "ぎ", "ぐ", "げ", "ご",
	"さ", "し", "す", "せ", "そ",
	"ざ", "じ", "ず", "ぜ", "ぞ",
	"た", "ち", "つ", "て", "と",
	"だ", "ぢ", "づ", "で", "ど",
	"は", "ひ", "ふ", "へ", "ほ",
	"ば", "び", "ぶ", "べ", "ぼ",
	"ぱ", "ぴ", "ぷ", "ぺ", "ぽ",
	"ま", "み", "む", "め", "も",
	"な", "に", "ぬ", "ね", "の",
	"ら", "り", "る", "れ", "ろ",
	"や",       "ゆ",      "よ",
	"わ", "を", "ん",
	""
};

static char * latin[] = {
	 "a",  "i",  "u",  "e",  "o",
	"ka", "ki", "ku", "ke", "ko",
	"ga", "gi", "gu", "ge", "go",
	"sa", "si", "su", "se", "so",
	"za", "zi", "zu", "ze", "zo",
	"ta", "ti", "tu", "te", "to",
	"da", "di", "du", "de", "do",
	"ha", "hi", "hu", "he", "ho",
	"ba", "bi", "bu", "be", "bo",
	"pa", "pi", "pu", "pe", "po",
	"ma", "mi", "mu", "me", "mo",
	"na", "ni", "nu", "ne", "no",
	"ra", "ri", "ru", "re", "ro",
	"ya",       "yu",       "yo",
	"wa", "wo", "n",
	""
};

typedef struct _reading {
	char * moracode;
	char * moras;
	struct _reading * next;
} reading;

typedef struct _kanji {
	char * glyph;
	char * jiscode;
	int jouyou;
	int used;
	struct _reading * r;
	struct _kanji * next;
} kanji;

char * transliterate(char * reading);
char * call_mora(char * reading);
char * append_string(char * x, char * y);

void free_kanji(kanji * klist);
kanji * readkanjidic(FILE * kanjidic);
int compilekanji(kanji * klist);
