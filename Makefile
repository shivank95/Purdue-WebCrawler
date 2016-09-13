
all: gethttp git-commit webcrawler

gethttp: gethttp.cpp openhttp.cpp SimpleHTMLParser.cpp
	g++ -o gethttp -g gethttp.cpp openhttp.cpp SimpleHTMLParser.cpp
	
webcrawler: webcrawler.cpp openhttp.cpp SimpleHTMLParser.cpp
	g++ -o webcrawler -g webcrawler.cpp openhttp.cpp SimpleHTMLParser.cpp

git-commit:
	git add Makefile *.h *.cpp  >> .local.git.out
	git commit -a -m "Commit web-crawler" >> .local.git.out

clean:
	rm -f *.o gethttp core
