all: 
	clang++ src/main.cpp -std=c++11 -stdlib=libc++ -lsdl2 -lsdl2_image -o minesweeper
