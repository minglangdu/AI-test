.SILENT: goal
goal:
	-g++ sdl.cpp -std=c++11 -c 
	-g++ main.cpp -std=c++11 -c
	-g++ sdl.o main.o -o main -I include -L lib -l SDL2-2.0.0 -std=c++11
	./main
	rm -rf sdl.o
	rm -rf main.o
	rm -rf main