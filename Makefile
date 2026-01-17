CXX=g++
CXXFLAGS=-std=c++11 -O2 -Wall -Wpedantic -I/opt/homebrew/include
LIBS=-lSDL2-2.0.0
LDFLAGS=-L/opt/homebrew/lib

.PHONY: all clean run
all: main
main: sdl.o main.o ai.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) sdl.o main.o ai.o -o main
sdl.o: sdl.cpp
	$(CXX) -c $(CXXFLAGS) sdl.cpp
ai.o: ai.cpp
	$(CXX) -c $(CXXFLAGS) ai.cpp
main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) main.cpp
run: main
	./main
clean:
	rm -f *.o
	rm main
	rm networks/agent.csv
