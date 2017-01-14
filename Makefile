default:
	mkdir -p test bin

install: rgb.o vector.o matrix.o ray.o camera.o windowing.o \
				sphere.o plane.o \
				scene.o shade.o quadtree.o shoot.o \
				octree_quantizer.o bmp.o tga.o
	gcc raytracing.c -o bin/raytracing \
				rgb.o vector.o matrix.o ray.o camera.o windowing.o \
				sphere.o plane.o \
				scene.o shade.o quadtree.o shoot.o \
				octree_quantizer.o bmp.o tga.o \
				-lm

rgb.o: rgb.h rgb.c
	gcc -c rgb.c

vector.o: datatypes.h vector.h vector.c
	gcc -c vector.c

vector_test: datatypes.h vector.h vector.c matrix.h matrix.c
	gcc -c vector.c
	gcc tests/vector_test.c -o bin/vector_test vector.o matrix.o -lm

matrix.o: datatypes.h matrix.h matrix.c
	gcc -c matrix.c

matrix_test: datatypes.h vector.h vector.c matrix.h matrix.c
	gcc -c vector.c
	gcc -c matrix.c
	gcc tests/matrix_test.c -o bin/matrix_test vector.o matrix.o -lm

ray.o: datatypes.h vector.h vector.c matrix.h matrix.c ray.h ray.c
	gcc -c ray.c

sphere.o: datatypes.h rgb.h rgb.c vector.h vector.c matrix.h matrix.c sphere.h sphere.c
	gcc -c sphere.c

octree_quantizer.o: rgb.h rgb.c octree_quantizer.h octree_quantizer.c
	gcc -c octree_quantizer.c

octree_quantizer_test: octree_quantizer.o
	gcc -c bmp.c
	gcc -c tga.c
	gcc -c pcx.c
	gcc tests/octree_quantizer_test.c -o bin/octree_quantizer_test octree_quantizer.o bmp.o tga.o pcx.o -lm

bmp.o: helpers.h rgb.h rgb.c bmp.h bmp.c
	gcc -c bmp.c

windowing.o: datatypes.h windowing.h windowing.c
	gcc -c windowing.c

camera.o: datatypes.h vector.h vector.c matrix.h matrix.c camera.h camera.c
	gcc -c camera.c

camera_test: datatypes.h windowing.h windowing.c camera.h camera.c
	gcc -c camera.c
	gcc -c windowing.c
	gcc tests/camera_test.c -o bin/camera_test vector.o matrix.o windowing.o camera.o -lm

tga.o: rgb.h rgb.c bmp.h bmp.c tga.h tga.c
	gcc -c tga.c

plane.o: datatypes.h rgb.h rgb.c vector.h vector.c matrix.h matrix.c plane.h plane.c
	gcc -c plane.c

scene.o: datatypes.h scene.h scene.c
	gcc -c scene.c

shade.o: datatypes.h rgb.h rgb.c ray.h ray.c
	gcc -c shade.c

shoot.o: datatypes.h rgb.h rgb.c shade.h shade.c shoot.h shoot.c
	gcc -c shoot.c

scene_test: datatypes.h scene.h scene.c rgb.h rgb.c vector.h vector.c matrix.h matrix.c camera.h camera.c sphere.h sphere.c plane.h plane.c ray.h ray.c
	gcc -c scene.c
	gcc -c vector.c
	gcc -c matrix.c
	gcc -c camera.c
	gcc -c sphere.c
	gcc -c plane.c
	gcc -c rgb.c
	gcc -c ray.c
	gcc -c windowing.c
	gcc tests/scene_test.c -o bin/scene_test vector.o matrix.o ray.o windowing.o camera.o rgb.o sphere.o plane.o scene.o -lm

quadtree.o: rgb.h rgb.c datatypes.h quadtree.h quadtree.c
	gcc -c quadtree.c

quadtree_test: helpers.h rgb.h rgb.c datatypes.h quadtree.h quadtree.c
	gcc -c rgb.c
	gcc -c quadtree.c
	gcc tests/quadtree_test.c -o bin/quadtree_test rgb.o quadtree.o -lm

clean:
	rm -f *.o
