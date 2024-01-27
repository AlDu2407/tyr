CC = g++
CXXFLAGS = -std=c++20 -c -g -Wall -Wextra
DEPS = 

all: main

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

main: main.o
	$(CC) main.o -o main

clean:
	rm main
	rm *.o