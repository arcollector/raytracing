default: rgb.o vector.o matrix.o ray.o sphere.o octree_quantizer.o bmp.o windowing.o camera.o tga.o plane.o scene.o
	gcc raytracing.c -o bin/raytracing rgb.o vector.o matrix.o ray.o sphere.o octree_quantizer.o bmp.o windowing.o camera.o tga.o plane.o scene.o -lm

rgb.o: rgb.h rgb.c
	gcc -c rgb.c

vector.o: datatypes.h vector.h vector.c
	gcc -c vector.c

matrix.o: datatypes.h matrix.h matrix.c
	gcc -c matrix.c

ray.o: datatypes.h vector.h vector.c matrix.h matrix.c ray.h ray.c
	gcc -c ray.c

sphere.o: datatypes.h rgb.h rgb.c vector.h vector.c matrix.h matrix.c sphere.h sphere.c
	gcc -c sphere.c

octree_quantizer.o: rgb.h rgb.c octree_quantizer.h octree_quantizer.c
	gcc -c octree_quantizer.c

bmp.o: rgb.h rgb.c bmp.h bmp.c
	gcc -c bmp.c

windowing.o: datatypes.h windowing.h windowing.c
	gcc -c windowing.c

camera.o: datatypes.h vector.h vector.c matrix.h matrix.c camera.h camera.c
	gcc -c camera.c

tga.o: rgb.h rgb.c bmp.h bmp.c tga.h tga.c
	gcc -c tga.c

plane.o: datatypes.h rgb.h rgb.c vector.h vector.c matrix.h matrix.c plane.h plane.c
	gcc -c plane.c

scene.o: datatypes.h scene.h scene.c
	gcc -c scene.c

scene_test: datatypes.h scene.h scene.c scene_test.c rgb.h rgb.c vector.h vector.c matrix.h matrix.c camera.h camera.c sphere.h sphere.c plane.h plane.c ray.h ray.c
	gcc -c scene.c
	gcc -c vector.c
	gcc -c matrix.c
	gcc -c camera.c
	gcc -c sphere.c
	gcc -c plane.c
	gcc -c rgb.c
	gcc -c ray.c
	gcc -c windowing.c
	gcc scene_test.c -o bin/scene_test vector.o matrix.o ray.o windowing.o camera.o rgb.o sphere.o plane.o scene.o -lm

clean:
	rm -f *.o

