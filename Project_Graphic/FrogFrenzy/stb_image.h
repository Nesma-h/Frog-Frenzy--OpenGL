// stb_image - v2.28 - public domain image loader
// Minimal version for texture loading in OpenGL
// Full version: https://github.com/nothings/stb/blob/master/stb_image.h

#ifndef STB_IMAGE_H
#define STB_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif

#define STBI_VERSION 1

enum
{
   STBI_default = 0,
   STBI_grey       = 1,
   STBI_grey_alpha = 2,
   STBI_rgb        = 3,
   STBI_rgb_alpha  = 4
};

extern stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
extern void stbi_image_free(void *retval_from_stbi_load);
extern const char *stbi_failure_reason(void);
extern void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

#ifdef STB_IMAGE_IMPLEMENTATION

static int stbi__vertically_flip_on_load = 0;

void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip)
{
   stbi__vertically_flip_on_load = flag_true_if_should_flip;
}

const char *stbi_failure_reason(void)
{
   return "unknown error";
}

void stbi_image_free(void *retval_from_stbi_load)
{
   free(retval_from_stbi_load);
}

// Simple BMP loader only (no external dependencies)
static stbi_uc* stbi__load_bmp(FILE *f, int *x, int *y, int *comp, int req_comp)
{
   unsigned char header[54];
   if (fread(header, 1, 54, f) != 54) return NULL;
   if (header[0] != 'B' || header[1] != 'M') return NULL;

   int width  = *(int*)&header[18];
   int height = *(int*)&header[22];
   int bpp    = *(short*)&header[28];
   if (bpp != 24) return NULL;

   int row_padded = (width * 3 + 3) & (~3);
   stbi_uc *data = (stbi_uc*)malloc(width * height * 3);
   if (!data) return NULL;

   fseek(f, *(int*)&header[10], SEEK_SET);

   for (int i = height - 1; i >= 0; i--) {
      fread(data + i * width * 3, 3, width, f);
      fseek(f, row_padded - width * 3, SEEK_CUR);
      // BGR -> RGB
      for (int j = 0; j < width; j++) {
         stbi_uc tmp = data[i*width*3 + j*3];
         data[i*width*3 + j*3] = data[i*width*3 + j*3 + 2];
         data[i*width*3 + j*3 + 2] = tmp;
      }
   }

   *x = width;
   *y = height;
   *comp = 3;
   return data;
}

stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels)
{
   FILE *f = fopen(filename, "rb");
   if (!f) return NULL;
   stbi_uc *result = stbi__load_bmp(f, x, y, channels_in_file, desired_channels);
   fclose(f);
   return result;
}

#endif // STB_IMAGE_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // STB_IMAGE_H
