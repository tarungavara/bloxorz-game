all: sample2D

sample2D: Sample_GL3_2D.cpp
	g++ -g -o sample2D Sample_GL3_2D.cpp -lglfw -lGL -lGLEW -ldl -lSOIL -I/usr/include/freetype2/ -L/usr/local/lib -lfreetype -std=c++11

clean:
	rm sample2D
