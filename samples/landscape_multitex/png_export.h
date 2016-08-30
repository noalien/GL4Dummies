#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>


typedef struct {
	float red;
	float green;
	float blue;
} pixel_t;

typedef struct  {
	pixel_t *pixels;
	size_t width;
	size_t height;
} bitmap_t;

pixel_t * pixel_at (bitmap_t * bitmap, int x, int y);
int save_png_to_file (bitmap_t *bitmap, const char *path);
float pix (float **tab, int x, int y, float min_height, float max_height);
void export (const char* filename, float ** tab, int taille, float min_height, float max_height);

