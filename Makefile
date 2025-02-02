.SILENT: goal
goal:
	g++ sdl.cpp -std=c++11 -c
	g++ main.cpp -std=c++11 -c
	-g++ sdl.o main.o -o main -std=c++11
	-./main
	rm -rf sdl.o
	rm -rf main.o