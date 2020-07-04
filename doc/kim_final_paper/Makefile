latex = latex
pdflatex = pdflatex
bibtex = bibtex

title = usenix
paper = $(title).tex

all:
	$(pdflatex) -synctex=1 -interaction=nonstopmode $(title)
	$(bibtex) $(title)
	$(pdflatex) -synctex=1 -interaction=nonstopmode $(title)
	$(pdflatex) -synctex=1 -interaction=nonstopmode $(title)

clean:
	rm $(title).out $(title).blg $(title).bbl $(title).aux $(title).toc $(title).log $(title).pdf
