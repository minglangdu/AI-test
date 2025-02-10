.SILENT: goal
goal:
	-g++ sdl.cpp -std=c++11 -c 
	-g++ main.cpp -std=c++11 -c
	-g++ ai.cpp -std=c++11 -c
	-g++ sdl.o main.o ai.o -o main \
	-I include -I ./Eigen -L lib -l SDL2-2.0.0 -std=c++11
	rm -rf sdl.o
	rm -rf ai.o
	rm -rf main.o
	-./main
	rm -rf main