#include "png_export.h"

pixel_t * pixel_at (bitmap_t * bitmap, int x, int y) {

	return bitmap->pixels + bitmap->width * y + x;
}


int save_png_to_file (bitmap_t *bitmap, const char *path) {

	FILE * fp;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	size_t x, y;
	png_byte ** row_pointers = NULL;

	int status = -1;
	int pixel_size = 3;
	int depth = 8;

	fp = fopen (path, "wb");
	if (! fp) {

		goto fopen_failed;
	}

	png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {

		goto png_create_write_struct_failed;
	}

	info_ptr = png_create_info_struct (png_ptr);
	if (info_ptr == NULL) {

		goto png_create_info_struct_failed;
	}

	//si ca fonctionne pas

	if (setjmp (png_jmpbuf (png_ptr))) {

		goto png_failure;
	}

	//les attributs de tout ce bordel

	png_set_IHDR (png_ptr, info_ptr, bitmap->width, bitmap->height, depth, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	//on initialize chaque colonne

	row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));

	for (y = 0; y < bitmap->height; ++y) {

		png_byte *row = png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
		row_pointers[y] = row;

		for (x = 0; x < bitmap->width; ++x) {

			pixel_t * pixel = pixel_at (bitmap, x, y);
			*row++ = pixel->red;
			*row++ = pixel->green;
			*row++ = pixel->blue;
		}
	}

	png_init_io (png_ptr, fp);
	png_set_rows (png_ptr, info_ptr, row_pointers);
	png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	status = 0;

	for (y = 0; y < bitmap->height; y++) {

		png_free (png_ptr, row_pointers[y]);
	}
	png_free (png_ptr, row_pointers);

png_failure:
png_create_info_struct_failed:
	png_destroy_write_struct (&png_ptr, &info_ptr);
png_create_write_struct_failed:
	fclose (fp);
fopen_failed:
	return status;
}



float pix (float ** tab, int x, int y, float min_height, float max_height) {

	// printf("%f\n", (tab[x][y] - min_height) / (max_height - min_height));
	return (tab[x][y] - min_height) / (max_height - min_height) * 255 ;
}

void export (const char* filename, float** tab, int taille, float min_height, float max_height) {

	bitmap_t image;
	uint32_t x;
	uint32_t y;

	image.width = taille;
	image.height = taille;
	image.pixels = calloc (image.width * image.height, sizeof (pixel_t));

	for (y = 0; y < image.height; y++) {

		for (x = 0; x < image.width; x++) {

			pixel_t * pixel = pixel_at (& image, x, y);
			pixel->red = /*(x + y) / (float)image.height * 128 ; */pix (tab, x, y, min_height, max_height);
			pixel->green = 0;//pixel->red;
			pixel->blue = 0;//pixel->red;
		}
	}

	save_png_to_file (& image, filename);
}
