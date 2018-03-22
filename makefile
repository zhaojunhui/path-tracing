all: main clear

vector3.o: vector3.h vector3.cpp
	g++ -c vector3.cpp -I/usr/local/include -O3

color.o: color.h color.cpp
	g++ -c color.cpp -I/usr/local/include -O3

AABB.o: AABB.h AABB.cpp
	g++ -c AABB.cpp -I/usr/local/include -O3

png.o: png.h png.cpp
	g++ -c png.cpp -I/usr/local/include -O3

object.o: object.h object.cpp
	g++ -c object.cpp -I/usr/local/include -O3 -std=c++11

light.o: light.h light.cpp
	g++ -c light.cpp  -I/usr/local/include -O3

camera.o: camera.h camera.cpp
	g++ -c camera.cpp  -I/usr/local/include -O3

scene.o: scene.h scene.cpp
	g++ -c scene.cpp  -I/usr/local/include -O3

raytracer.o: raytracer.h raytracer.cpp
	g++ -c raytracer.cpp  -I/usr/local/include -O3

lodepng.o: lodepng.h lodepng.cpp
	g++ -c lodepng.cpp  -I/usr/local/include -O3

main:   main.cpp lodepng.o vector3.o color.o AABB.o png.o object.o light.o camera.o scene.o raytracer.o
	g++ main.cpp lodepng.o vector3.o color.o AABB.o png.o object.o light.o camera.o scene.o raytracer.o -o main  -I/usr/local/include -O3

clear:
	rm *.o
