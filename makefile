default: rgb vector matrix ray sphere octree_quantizer bmp windowing camera tga plane
	gcc raytracing.c -o raytracing rgb.o vector.o matrix.o ray.o sphere.o octree_quantizer.o bmp.o windowing.o camera.o tga.o plane.o -lm

rgb:
	gcc -c rgb.c
  
vector:
	gcc -c vector.c

matrix:
	gcc -c matrix.c

ray:
	gcc -c ray.c

sphere:
	gcc -c sphere.c

octree_quantizer:
	gcc -c octree_quantizer.c

bmp:
	gcc -c bmp.c

windowing:
	gcc -c windowing.c

camera:
	gcc -c camera.c

tga:
	gcc -c tga.c

plane:
	gcc -c plane.c
