struct _readings {
	char * moracode;
	char * moras;
	struct _readings * next;
}

struct _kanji {
	char * kanji;
	char * jis;
	int jouyou;
	struct _readings * read;
	struct _kanji * next;
}
