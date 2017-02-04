default:
	mkdir -p tests bin

clean:
	rm -f *.o *.bmp *.BMP

install: rgb.o vector.o matrix.o ray.o camera.o windowing.o \
				sphere.o plane.o \
				texture.o \
				scene.o shade.o shoot.o \
				octree_quantizer.o bmp.o tga.o \
				bbox.o
	gcc raytracing.c -o bin/raytracing \
				rgb.o vector.o matrix.o ray.o camera.o windowing.o \
				texture.o \
				sphere.o plane.o \
				scene.o shade.o shoot.o \
				octree_quantizer.o bmp.o tga.o \
				bbox.o \
				-lm -lpthread -Wall -g

# ---- Global --------------------

rgb.o: rgb.h rgb.c
	gcc -c rgb.c

# ---- Image related tasks -----

octree_quantizer.o: rgb.h rgb.c \
										octree_quantizer.h octree_quantizer.c
	gcc -c octree_quantizer.c

octree_quantizer_test: octree_quantizer.o
	gcc -c bmp.c
	gcc -c tga.c
	gcc -c pcx.c
	gcc tests/octree_quantizer_test.c -o bin/octree_quantizer_test octree_quantizer.o bmp.o tga.o pcx.o -lm

bmp.o: rgb.h rgb.c \
 			bmp.h bmp.c
	gcc -c bmp.c

tga.o: rgb.h rgb.c bmp.h bmp.c \
			tga.h tga.c
	gcc -c tga.c

# ------ Ray tracing stuff -----------

vector.o: matrix.h matrix.c \
				vector.h vector.c
	gcc -c vector.c

matrix.o: matrix.h matrix.c
	gcc -c matrix.c

texture.o: rgb.h rgb.c vector.h vector.c \
					texture.h texture.c
	gcc -c texture.c

ray.o: vector.h vector.c matrix.h matrix.c \
			ray.h ray.c
	gcc -c ray.c

sphere.o: helpers.h rgb.h rgb.c vector.h vector.c matrix.h matrix.c ray.h ray.c camera.h camera.c texture.h texture.c \
					sphere.h sphere.c
	gcc -c sphere.c

plane.o: helpers.h rgb.h rgb.c vector.h vector.c matrix.h matrix.c ray.h ray.c camera.h camera.c texture.h texture.c \
				plane.h plane.c
	gcc -c plane.c

windowing.o: vector.h vector.c matrix.h matrix.c \
						windowing.h windowing.c
	gcc -c windowing.c

camera.o: vector.h vector.c matrix.h matrix.c windowing.h windowing.c \
					camera.h camera.c
	gcc -c camera.c

scene.o: rgb.h rgb.c vector.h vector.c matrix.h matrix.c camera.h camera.c object.h texture.h texture.c \
				sphere.h sphere.c \
				plane.h plane.c \
				scene.h scene.c
	gcc -c scene.c

shade.o: rgb.h rgb.c vector.h vector.c ray.h ray.c camera.h camera.c texture.c texture.h bbox.h bbox.c \
				shade.h shade.c
	gcc -c shade.c

shoot.o: helpers.h rgb.h rgb.c vector.h vector.h ray.h ray.c camera.h camera.c object.h shade.h shade.c scene.h scene.c texture.h texture.c bbox.h bbox.c\
				shoot.h shoot.c
	gcc -c shoot.c

bbox.o: vector.h vector.c object.h sphere.h sphere.c \
				bbox.h bbox.c
	gcc -c bbox.c

# ------ Tests ----------------

vector_test:
	gcc -c vector.c
	gcc -g tests/vector_test.c -o bin/vector_test vector.o matrix.o -lm

matrix_test:
	gcc -c matrix.c
	gcc -g tests/matrix_test.c -o bin/matrix_test matrix.o -lm

camera_test:
	gcc -c camera.c
	gcc -c windowing.c
	gcc -g tests/camera_test.c -o bin/camera_test vector.o matrix.o windowing.o camera.o -lm

scene_test:
	gcc -c scene.c
	gcc -c vector.c
	gcc -c matrix.c
	gcc -c camera.c
	gcc -c windowing.c
	gcc -c sphere.c
	gcc -c plane.c
	gcc -c rgb.c
	gcc -c texture.c
	gcc -c ray.c
	gcc -g tests/scene_test.c -o bin/scene_test vector.o matrix.o camera.o rgb.o sphere.o plane.o scene.o texture.o windowing.o ray.o -lm

bbox_test:
	gcc -c bbox.c
	gcc -c scene.c
	gcc -c vector.c
	gcc -c matrix.c
	gcc -c camera.c
	gcc -c windowing.c
	gcc -c sphere.c
	gcc -c plane.c
	gcc -c rgb.c
	gcc -c texture.c
	gcc -c ray.c
	gcc -g tests/bbox_test.c -o bin/bbox_test rgb.o vector.o matrix.o ray.o sphere.o camera.o windowing.o bbox.o texture.o -lm

split_test:
	gcc -c bbox.c
	gcc -c scene.c
	gcc -c vector.c
	gcc -c matrix.c
	gcc -c camera.c
	gcc -c windowing.c
	gcc -c sphere.c
	gcc -c plane.c
	gcc -c rgb.c
	gcc -c texture.c
	gcc -c ray.c
	gcc -g tests/split_test.c -o bin/split_test rgb.o vector.o matrix.o ray.o sphere.o camera.o windowing.o bbox.o texture.o -lm
