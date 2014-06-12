all: programming.pdf intro.pdf

intro.pdf: intro.tex
	pdflatex intro
	pdflatex intro

programming.pdf: programming.tex
	pdflatex programming
	pdflatex programming

clean:
	rm -f *.pdf *.aux *.out *.log
	
