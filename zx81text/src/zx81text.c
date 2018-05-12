/*
ZX81TEXT: Transform the output of AGEPLIST or WMAPLIST into a BMP file.

Copyright (C) 2010 Andre de Leiradella.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "zx81rom.h"
#include "xltables.h"

typedef struct
{
  char* line;
  void* previous;
} line_t;

static int char_index[256] =
{
       /*   x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   XB   XC   XD   XE   XF */
  /* 0x */  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
  /* 1x */  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
  /* 2x */   0,  -1,  11,  -1,  13,  -1,  -1,  -1,  16,  17,  23,  21,  26,  22,  27,  24, 
  /* 3x */  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  14,  25,  19,  20,  18,  15, 
  /* 4x */  -1,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52, 
  /* 5x */  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  -1,  -1,  -1,  -1,  -1, 
  /* 6x */  -1, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 
  /* 7x */ 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,  -1,  -1,  -1,  -1,  -1, 
  /* 8x */  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
  /* 9x */  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
  /* Ax */  -1,  -1,  -1,  12,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
  /* Bx */   8,   9,  10, 136, 137, 138,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
  /* Cx */  -1,   1,   2,   3,   4,   5,   6,   7, 135, 134, 133, 132, 131, 130, 129, 128, 
  /* Dx */  -1,  -1, 139,  -1, 141, 140,  -1,  -1, 144, 145, 151, 149, 154, 150, 155, 152, 
  /* Ex */ 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 142, 153, 147, 148, 146, 143, 
  /* Fx */  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 
};

static void write8(FILE* out, uint8_t x)
{
  fwrite(&x, 1, 1, out);
}

static void write16(FILE* out, uint16_t x)
{
  write8(out, x & 0xff);
  write8(out, x >> 8);
}

static void write32(FILE* out, uint32_t x)
{
  write16(out, x & 0xffff);
  write16(out, x >> 16);
}

static void usage(FILE* out)
{
  fprintf(out, "ZX81TEXT - Generates BMP images with text using the ZX-81 font.\n\n");
  fprintf(out, "Usage: zx81text [-h] [-o output]\n\n");
  fprintf(out, "-o    Output bitmap to file \"output\"\n\n");
  fprintf(out, "The purpose of this software is to generate bitmap images from listings\n");
  fprintf(out, "produced by AGEPLIST and WMAPLIST.\n");
}

int main(int argc, const char* argv[])
{
  // check execution without arguments
  if (argc < 2)
  {
    usage(stderr);
    return -1;
  }
  
  // check for -h
  if (!strcmp(argv[1], "-h"))
  {
    usage(stdout);
    return 0;
  }
  
  // check for required argument
  if (strcmp(argv[1], "-o"))
  {
    fprintf(stderr, "Missing -o option\n");
    return -1;
  }
  if (argc < 3)
  {
    fprintf(stderr, "Missing argument to -o\n");
    return -1;
  }
  
  // read the file into memory, maximum line width is 8 KiB
  line_t* last_line = NULL;
  char line[8192];
  int total_lines = 0;
  int max_columns = 0;
  while (fgets(line, sizeof(line), stdin) != NULL)
  {
    // force string termination
    line[sizeof(line) - 1] = 0;
    // remove \r and \n from the end of the string
    char* aux = line + strlen(line) - 1;
    while (aux >= line && (*aux == '\r' || *aux == '\n'))
    {
      aux--;
    }
    aux[1] = 0;
    // add it to the dynamic array
    line_t* next_line = (line_t*)malloc(sizeof(line_t));
    next_line->previous = last_line;
    last_line = next_line;
    next_line->line = strdup(line);
    total_lines++;
    int len = strlen(line);
    if (len > max_columns)
    {
      max_columns = len;
    }
  }

  // check if there is text to output
  if (total_lines == 0 || max_columns == 0)
  {
    fprintf(stderr, "No text to output\n");
    return -1;
  }
  
  // compute width (with padding) and height
  int width = (max_columns * 8 + 31) & ~31;
  int height = total_lines * 8;
  
  // setup output file
  FILE* output = fopen(argv[2], "wb");
  if (output == NULL)
  {
    fprintf(stderr, "Error opening output file: %s\n", strerror(errno));
    return -1;
  }

	// write the Win32 BMP file header (14 bytes)
	write8(output, 'B');                               // magic
  write8(output, 'M');                               // magic
	write32(output, 14 + 40 + width / 8 * height + 8); // bfSize (file size)
  write32(output, 0);                                // bfReserved1 & bfReserved2
	write32(output, 14 + 40 + 8);                      // bfOffBits

	// write the Win32 BITMAPINFOHEADER struct (40 bytes)
	write32(output, 40);                 // biSize
	write32(output, max_columns * 8);    // biWidth
	write32(output, height);             // biHeight
	write16(output, 1);                  // biPlanes
	write16(output, 1);                  // biBitCount
	write32(output, 0);                  // biCompression
	write32(output, width / 8 * height); // biSizeImage
	write32(output, 72 * 10000 / 254);   // biXPelsPerMeter
	write32(output, 72 * 10000 / 254);   // biYPelsPerMeter
	write32(output, 2);                  // biClrUsed
	write32(output, 2);                  // biClrImportant
  
  // write the color palette
  write32(output, 0x00ffffff);
  write32(output, 0);
	
  // write the pixels
  const uint8_t* char_table = rom + 0x1e00;
  line_t* current = last_line;
  while (current != NULL)
  {
    // generate eight lines of pixels for each string
    int line_count;
    for (line_count = 7; line_count >= 0; line_count--)
    {
      // iterate over the string
      int len = strlen(current->line);
      int column;
      for (column = 0; column < len; column++)
      {
        // get the index of the character into the character table
        int index = char_index[(uint8_t)current->line[column]];
        // if it's an invalid character, use a '?'
        if (index == -1)
        {
          index = 15;
        }
        int invert = 0;
        if (index >= 128)
        {
          index -= 128;
          invert = 0xff;
        }
        // output the bits of the character corresponding to line_count
        write8(output, char_table[index * 8 + line_count] ^ invert);
      }
      // complete the horizontal line
      while (column++ < max_columns)
      {
        write8(output, 0);
      }
      // pad the horizontal line
      for (column = width / 8 - max_columns; column != 0; column--)
      {
        write8(output, 0);
      }
    }
    current = current->previous;
  }
  
  // all done, free the lines, close output file and exit
  current = last_line;
  while (current != NULL)
  {
    free(current->line);
    last_line = current;
    current = current->previous;
    free(last_line);
  }
  fclose(output);
  return 0;
}
