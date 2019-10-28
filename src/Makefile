CC = gcc
CFLAGS = -g -Wall
OBJECTS = main.o postingList.o trie.o WordList.o worker.o jobExecutor.o txInfo.o dirsMap.o acutil.o 

jobExecutor: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o jobExecutor

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

postingList.o: postingList.c
	$(CC) $(CFLAGS) -c postingList.c

trie.o: trie.c
	$(CC) $(CFLAGS) -c trie.c

WordList.o: WordList.c
	$(CC) $(CFLAGS) -c WordList.c

dirsMap.o: dirsMap.c
	$(CC) $(CFLAGS) -c dirsMap.c	

txInfo.o: txInfo.c
	$(CC) $(CFLAGS) -c txInfo.c

acutil.o: acutil.c
	$(CC) $(CFLAGS) -c acutil.c

.PHONY: clean

clean:
	rm -f jobExecutor $(OBJECTS) 
