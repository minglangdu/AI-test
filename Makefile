.SILENT: goal
goal:
	echo "Compiling sdl.cpp..."
	-g++ sdl.cpp -std=c++11 -c \
	-I /opt/homebrew/include
	echo "Done." 
	echo "Compiling ai.cpp..."
	-g++ ai.cpp -std=c++11 -c \
	-I /opt/homebrew/include
	echo "Done."
	echo "Compiling main.cpp..."
	-g++ main.cpp -std=c++11 -c \
	-I /opt/homebrew/include
	echo "Done."
	echo "Compiling together..."
	-g++ sdl.o main.o ai.o -o main \
	-I /opt/homebrew/include -I ./Eigen -L /opt/homebrew/lib -l SDL2-2.0.0 -std=c++11
	echo "Done."
	echo "Cleaning..."
	rm -rf sdl.o
	rm -rf ai.o
	rm -rf main.o
	echo "Running..."
	-./main
	echo "Cleaning..."
	rm -rf main
	echo "Done."