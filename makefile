plagiarismCatcher: main.o
	g++ -oplagiarismCatcher main.o
main.o: main.cpp
	g++ -c -std=c++11 main.cpp
