/*
AGEPLIST: A Good Enough PLISTer

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
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "xltables.h"

static void usage(FILE* out)
{
  fprintf(out, "AGEPLIST - A Good Enough P LIST program.\n");
  fprintf(out, "Copyright (C) 2010 Andre de Leiradella. Released under the GPL.\n\n");
  fprintf(out, "Usage: ageplist [-h] [-c] [-z] [-w width] [-s n] [-f] [-a] [-o output] input.p\n\n");
  fprintf(out, "-h    Show this help screen\n");
  fprintf(out, "-c    Show the current line cursor (toggle, default: no)\n");
  fprintf(out, "-z    Use the ZX-81.TTF font (toggle, default: no)\n");
  fprintf(out, "-w    Maximum number of character per line (default: 32)\n");
  fprintf(out, "-s    Set the first line to list (default: 0)\n");
  fprintf(out, "-f    Don't stop the listing on spurious program endings (toggle, default: no)\n");
  fprintf(out, "-a    Accurate (turns -c and -z on, -w to 32 and -f off (default: no)\n");
  fprintf(out, "-o    Output listing to file \"output\" (default: stdout)\n\n");
}

static inline int peekb(const unsigned char* buffer, int addr)
{
  if (addr < 0 || addr > 65535)
  {
    return 0x76;
  }
  return buffer[addr];
}

static inline int peekw(const unsigned char* buffer, int addr)
{
  return peekb(buffer, addr) << 8 | peekb(buffer, addr + 1);
}

static inline void print(FILE* out, const char* what, int width, int* column, int* was_space)
{
  if (*what == ' ' && what[1] != 0 && *was_space)
  {
    what++;
  }
  while (*what != 0)
  {
    if (++(*column) == width)
    {
      fprintf(out, "\n");
      *column = 0;
    }
    fprintf(out, "%c", *what++);
  }
  *was_space = what[-1] == ' ';
}

int main(int argc, const char* argv[])
{
  // check execution without arguments
  if (argc < 2)
  {
    usage(stderr);
    return -1;
  }
  
  // configuration variables
  int show_cursor = 0;
  const char** table = table_ascii;
  const char* output_name = "<stdout>";
  FILE* output = stdout;
  const char* input_name = NULL;
  int width = 32;
  int start = 0;
  int full = 0;
  
  // process command line arguments
  int i;
  for (i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-c"))
    {
      show_cursor = !show_cursor;
    }
    else if (!strcmp(argv[i], "-z"))
    {
      table = table == table_ascii ? table_zx81 : table_ascii;
    }
    else if (!strcmp(argv[i], "-w"))
    {
      if ((i + 1) >= argc)
      {
        fprintf(stderr, "Missing argument to -w\n");
        return -1;
      }
      width = atoi(argv[++i]);
    }
    else if (!strcmp(argv[i], "-a"))
    {
      show_cursor = 1;
      table = table_zx81;
      width = 32;
      full = 0;
    }
    else if (!strcmp(argv[i], "-s"))
    {
      if ((i + 1) >= argc)
      {
        fprintf(stderr, "Missing argument to -s\n");
        return -1;
      }
      start = atoi(argv[++i]);
      if (start < 0 || start > 9999)
      {
        fprintf(stderr, "Invalid argument to -s, line must be between 0 and 9999\n");
        return -1;
      }
    }
    else if (!strcmp(argv[i], "-f"))
    {
      full = !full;
    }
    else if (!strcmp(argv[i], "-o"))
    {
      if ((i + 1) >= argc)
      {
        fprintf(stderr, "Missing argument to -o\n");
        return -1;
      }
      output_name = argv[++i];
    }
    else if (argv[i][0] != '-')
    {
      input_name = argv[i];
    }
    else if (!strcmp(argv[i], "-h"))
    {
      usage(stdout);
      return 0;
    }
    else
    {
      fprintf(stderr, "Unknown option: %s\n", argv[i]);
      return -1;
    }
  }
  
  // load input file
  FILE* input = fopen(input_name, "rb");
  if (input == NULL)
  {
    fprintf(stderr, "Error opening input file: %s\n", strerror(errno));
    return -1;
  }
  unsigned char buffer[65536];
  fread(buffer + 0x4009, 1, sizeof(buffer) - 0x4009, input);
  if (ferror(input))
  {
    fprintf(stderr, "Error reading input file: %s\n", strerror(errno));
    fclose(input);
    return -1;
  }
  fclose(input);
  
  // setup output file
  if (strcmp(output_name, "<stdout>"))
  {
    output = fopen(output_name, "wb");
    if (output == NULL)
    {
      fprintf(stderr, "Error opening output file: %s\n", strerror(errno));
      return -1;
    }
  }
  
  // list BASIC program
  int current = 0x407d; // address of first line
  int was_space = 0;    // true if last character was a space
  int column = 0;       // column counter
  int cursor = peekb(buffer, 0x400a) | peekb(buffer, 0x400b) << 8; // number of line with the cursor
  // a 0x76 marks the end of the program
  while (peekb(buffer, current) != 0x76)
  {
    // evaluate the next line
    int next_line = current + (peekb(buffer, current + 2) | peekb(buffer, current + 3) << 8) + 4;
    // get the line number
    int line = peekw(buffer, current);
    // listing ends if line number is >= 16384
    if (line >= 16384)
    {
      break;
    }
    // if line is less than the first line to list...
    if (line < start)
    {
      // restart loop in the next line
      current = next_line;
      continue;
    }
    // print line number plus cursor
    static const char* first_digit = " 123456789ABCDEFG"; // the first line digit can go up to G
    char line_number[5];
    snprintf(line_number, sizeof(line_number), "%c%3d", first_digit[line / 1000], line % 1000);
    line_number[sizeof(line_number) - 1] = 0;
    if (line_number[0] != ' ')
    {
      if (line_number[1] == ' ')
      {
        line_number[1] = '0';
      }
      if (line_number[2] == ' ')
      {
        line_number[2] = '0';
      }
    }
    fprintf(output, "%s%s", line_number, show_cursor && line == cursor ? table[0x92] : " ");
    // the last character was a space
    was_space = 1;
    // the line number always occupies four characters
    column = 4;
    // skip the line number and line length
    current += 4;
    // repeat until the end of the line
    while (peekb(buffer, current) != 0x76)
    {
      // 0x7e marks the start of floating-point constants
      if (peekb(buffer, current) == 0x7e)
      {
        // we skip it because the literal value to list follows the constant
        current += 6;
      }
      else
      {
        // output the character with the translation table
        // print updates column and was_space
        print(output, table[peekb(buffer, current)], width, &column, &was_space);
        // skip the character
        current++;
      }
    }
    // line ended, print a new line
    fprintf(output, "\n");
    // restart column at 0
    column = 0;
    // go to the next line
    if (full)
    {
      // for full listings we just go to the next line
      current = next_line;
    }
    else
    {
      // otherwise skip the 0x76 marking the end of the line
      current++;
    }
  }
  
  // all done, close output file and exit
  if (output != stdout)
  {
    fclose(output);
  }
  return 0;
}
