CC=g++
CXXFLAGS= -Wall -g -std=c++11

minisearch: main.o minisearch.o trie.o posting_list.o
	$(CC) $(CXXFLAGS) main.o minisearch.o trie.o posting_list.o -o minisearch

main.o: main.cpp minisearch.h
	$(CC) $(CXXFLAGS) -c main.cpp
	
minisearch.o: minisearch.cpp minisearch.h
	$(CC) $(CXXFLAGS) -c minisearch.cpp
	
trie.o: trie.cpp trie.h
	$(CC) $(CXXFLAGS) -c trie.cpp
	
posting_list.o: posting_list.cpp posting_list.h
	$(CC) $(CXXFLAGS) -c posting_list.cpp

.PHONY: clean

clean:
	rm -f minisearch main.o minisearch.o trie.o posting_list.o
