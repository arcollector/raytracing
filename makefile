default:
	gcc raytracing.c -o raytracing rgb.o vector.o matrix.o ray.o sphere.o octree_quantizer.o bmp.o windowing.o camera.o tga.o -lm

    
