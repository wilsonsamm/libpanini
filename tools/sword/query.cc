#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <swmgr.h>
#include <swmodule.h>
#include <markupfiltmgr.h>
#include <string>

using namespace::sword;
using namespace std;

string trim(string str) {
	// trim leading spaces
	size_t startpos = str.find_first_not_of(" \t\n");
	if( string::npos != startpos ) {
		str = str.substr( startpos );
	}
	// trim trailing spaces
	size_t endpos = str.find_last_not_of(" \t\n");
	if( string::npos != endpos )
	{
	    str = str.substr( 0, endpos+1 );
	}

	return str;
}

void outline(char * l1, string a, char * l2, string b) {

	string stra = trim(a);
	string strb = trim(b);
	
	std::cout << l1 << ":" << stra << endl;
	std::cout << l2 << ":" << strb << endl << endl;
}

void verse(char * lang1, SWModule *source, char * lang2, SWModule *target, string verse) {

	/* Look the verse up in the source module */
	source->setKey(verse.c_str());
	target->RenderText(); 

	/* Look the verse up in the target module */
	target->setKey(verse.c_str());
	target->RenderText();

	/* If both bible translations have the specified verse, then print them both
	 * out */
	if(strlen(source->RenderText()) == 0) return;
	if(strlen(target->RenderText()) == 0) return;
	outline(lang1, source->RenderText(), lang2, target->RenderText());
	return;
}
	

int main(int argc, char **argv)
{
	SWMgr library(new MarkupFilterMgr(FMT_PLAIN));
	SWModule *target;
	SWModule *source;

	if (argc != 5) {
		fprintf(stderr, "\nusage: query <language1> <module1> <language2> <module2> \n"
							 "\tExample: query english KJV czech CzeKMS\n\n");
		exit(-1);
	}

	source = library.getModule(argv[2]);
	if (!source) {
		fprintf(stderr, "sword/query could not find [%s].\n", argv[2]);
		exit(-1);
	}

	target = library.getModule(argv[4]);
	if (!target) {
		fprintf(stderr, "sword/query could not find [%s].\n", argv[4]);
		exit(-1);
	}

	std:cout << endl;
	
	/* Ideal verses for learning a language may be listed below. I have chosen
	 * some that consist of fairly simple sentences or fragments, to increase
	 * the chance that the source text may be understood by the Panini language
	 * module (because only then may something be learned from it)
	 */

	verse(argv[1], source, argv[3], target, "Matthew 6:11");
	verse(argv[1], source, argv[3], target, "Matthew 7:18");
	verse(argv[1], source, argv[3], target, "Matthew 8:23");
	verse(argv[1], source, argv[3], target, "Psalm 56:3");
	verse(argv[1], source, argv[3], target, "1 Thessalonians 5:16");
	verse(argv[1], source, argv[3], target, "Deuteronomy 5:19");
	/* These verses are chosen for their syntactic simplicity; you can add as
	 * many as you wish. */
	
	return 0;
}
