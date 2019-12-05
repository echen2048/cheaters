plagiarismCatcher: main.o
	g++ -otest main.o
main.o: main.cpp
	g++ -c main.cpp
