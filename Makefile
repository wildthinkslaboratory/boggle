CC = g++
CFLAGS = -O3

boggle : boggle.cpp board.cpp dict.cpp board.h dict.h util.h
	$(CC) $(CFLAGS) -o boggle boggle.cpp board.cpp dict.cpp

clean :
	rm ./boggle