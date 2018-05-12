/*
WMAPLIST: World's Most Accurate PLISTer

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
#include <errno.h>
#include "mem_mmu.h"
#include "simz80.h"
#include "zx81rom.h"
#include "xltables.h"

// some zx-81 system variables
#define ERR_NR 0x4000
#define FLAGS  0x4001
#define ERR_SP 0x4002
#define RAMTOP 0x4004
#define MODE   0x4006
#define PPC    0x4007
#define E_PPC  0x400a
#define D_FILE 0x400c
#define DF_CC  0x400e
#define NXTLIN 0x4029
#define S_POSN 0x4039
#define PRBUFF 0x403c

// globals used by the simulator
WORD af[2];
int af_sel;

struct ddregs regs[2];
int regs_sel;

WORD ir;
WORD ix;
WORD iy;
WORD sp;
WORD pc;
WORD IFF;

// dummy input/output callbacks
int in(unsigned int port)
{
  // return 0xff so no spurious key presses
  return 0xff;
}

void out(unsigned int port, unsigned char value)
{
}

static void usage(FILE* out)
{
  fprintf(out, "WMAPLIST - World's Most Accurate P LIST program.\n\n");
  fprintf(out, "Copyright (C) 2010 Andre de Leiradella. Released under the GPL.\n\n");
  fprintf(out, "Usage: wmaplist [-h] [-c] [-z] [-w width] [-s n] [-f] [-a] [-o output] input.p\n\n");
  fprintf(out, "-h    Show this help screen\n");
  fprintf(out, "-c    Show the current line cursor (toggle, default: no)\n");
  fprintf(out, "-z    Use the ZX-81.TTF font (toggle, default: no)\n");
  fprintf(out, "-w    Maximum number of character per line (default: 32)\n");
  fprintf(out, "-s    Set the first line to list (default: 0)\n");
  fprintf(out, "-f    Don't stop the listing on spurious program endings (toggle, default: no)\n");
  fprintf(out, "-a    Accurate (turns -c and -z on, -w to 32 and -f off (default: no)\n");
  fprintf(out, "-o    Output listing to file \"output\" (default: stdout)\n\n");
}

static void setup_simulation()
{
  // load ROM with ghosting
  memcpy(ram, rom, 8192);
  memcpy(ram + 8192, rom, 8192);
  // patch DISPLAY-5 to a no-op in ROM
  ram[0x02b5] = 0xc9;
  ram[0x02b5 + 8192] = 0xc9;
  // zero rest of RAM
  memset(ram + 0x4000, 0, 0xc000);

  // put stack values in place
  static const BYTE stack[] =
  {
    /*3FB0:*/ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB9, 0xC3, 0x8F, 0x02, 
    /*3FC0:*/ 0x1C, 0x19, 0x0E, 0x17, 0x00, 0xC0, 0x00, 0x00, 0x13, 0x17, 0x29, 0x17, 0x00, 0x00, 0x71, 0x17, 
    /*3FD0:*/ 0x73, 0x18, 0xC0, 0x43, 0xC2, 0x09, 0xC5, 0x43, 0xCB, 0x0E, 0xF3, 0x19, 0x1C, 0x1A, 0x05, 0x00, 
    /*3FE0:*/ 0x27, 0x1A, 0xC8, 0x0E, 0xF3, 0x19, 0xCF, 0x0E, 0xCB, 0x0E, 0xF3, 0x19, 0x01, 0x40, 0xBC, 0x43, 
    /*3FF0:*/ 0xC7, 0x12, 0x81, 0x02, 0x3B, 0x40, 0xFF, 0xFF, 0x80, 0x00, 0x85, 0x01, 0x76, 0x06, 0x00, 0x3E, 
  };
  memcpy(ram + 0x8000 - sizeof(stack), stack, sizeof(stack));
	
  // setup the registers
  regs[0].bc = 0x0080;
  regs[0].de = 0xffff;
  regs[0].hl = 0x403b;
  af[0]      = 0x0185;
  
  regs[1].bc = 0x8102;
  regs[1].de = 0x002b;
  regs[1].hl = 0x0000;
  af[1]      = 0xca89;
  
  ix = 0x0281;
  iy = 0x4000;
  ir = 0x1edf;
  sp = 0x7ffe;
  pc = 0x0676;
  
  IFF = 0;
  af_sel = regs_sel = 0;
  
  // setup system vars that are not saved in the P file
  ram[ERR_NR    ] = 0xff;
  ram[FLAGS     ] = 0x80;
  ram[ERR_SP    ] = 0xfc;
  ram[ERR_SP + 1] = 0x7f;
  ram[RAMTOP    ] = 0x00;
  ram[RAMTOP + 1] = 0x80;
  ram[MODE      ] = 0x00;
  ram[PPC       ] = 0xfe;
  ram[PPC    + 1] = 0xff;

  // now the state is a copy of a zx81 at the very ending of a LOAD command
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
  setup_simulation();
  FILE* input = fopen(input_name, "rb");
  if (input == NULL)
  {
    fprintf(stderr, "Error opening input file: %s\n", strerror(errno));
    return -1;
  }
  fread(ram + 0x4009, 1, 65536 - 0x4009, input);
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
  
  // if full list was required, we have to tweak the program to remove spurious program endings (0x76 0x76)
  if (full)
  {
    int current = 0x407d; // address of first line
    int target = current; // target position
    while (ram[current] != 0x76)
    {
      // evaluate the next line
      int next_line = current + (ram[current + 2] | ram[current + 3] << 8) + 4;
      // copy the line number and the line length to target
      for (i = 0; i < 4; i++)
      {
        ram[target++] = ram[current++];
      }
      // repeat until the end of the line
      while (ram[current] != 0x76)
      {
        // 0x7e marks the start of floating-point constants
        if (ram[current] == 0x7e)
        {
          // we skip it because the literal value to list follows the constant
          for (i = 0; i < 6; i++)
          {
            ram[target++] = ram[current++];
          }
        }
        else
        {
          ram[target++] = ram[current++];
        }
      }
      // copy the end of line
      ram[target++] = ram[current++];
      // check for spurious line endings
      if (ram[current] == 0x76 && current != next_line)
      {
        // jump to the next line so that we overwrite the spurious line ending
        current = next_line;
      }
    }
    // add the program ending
    ram[target] = 0x76;
  }
  
  // put a BASIC program into the printer buffer and make the BIOS resume to it after the load
  // 1 LIST VAL "0000"
  // 2 STOP
  static const BYTE program[] =
  {
    /*407D:*/ 0x00, 0x01, 0x09, 
    /*4080:*/ 0x00, 0xF0, 0xC5, 0x0B, 0x1C, 0x1C, 0x1C, 0x1C, 0x0B, 0x76, 0x00, 0x02, 0x02, 0x00, 0xE3, 0x76, 
    /*4090:*/ 0x76, 
  };
  memcpy(ram + PRBUFF, program, sizeof(program));
  // tell BIOS to resume running on out program
  ram[NXTLIN    ] = PRBUFF & 0xff;
  ram[NXTLIN + 1] = PRBUFF >> 8;
  
  // override starting line number
  char line_number[5];
  snprintf(line_number, sizeof(line_number), "%.4d", start);
  ram[PRBUFF +  7] = 0x1c + line_number[0] - '0';
  ram[PRBUFF +  8] = 0x1c + line_number[1] - '0';
  ram[PRBUFF +  9] = 0x1c + line_number[2] - '0';
  ram[PRBUFF + 10] = 0x1c + line_number[3] - '0';
  
  // save the E_PPC to show/hide the cursor
  int e_ppc = show_cursor ? ram[E_PPC] | ram[E_PPC + 1] << 8 : 65535;
  
  // resume simulation!
  ram[S_POSN    ] = 33; // 33 columns available in line (includes the new line)
  ram[S_POSN + 1] = 24; // 24 lines available in the screen
  int column = -1;      // column counter
  FASTREG PC = pc;      // the z80 program counter
  while (PC != 0x0cdc) // run util STOP command called
	{
    // Overwrite E_PPC to show/hide the cursor
    ram[E_PPC    ] = e_ppc & 0xff;
    ram[E_PPC + 1] = e_ppc >> 8;
    // get the address of the first character in the screen
    int d_file = ram[D_FILE] | ram[D_FILE + 1] << 8;
    d_file++;
    // hack the print position to the first character in the screen
    ram[DF_CC    ] = d_file & 0xff;
    ram[DF_CC + 1] = d_file >> 8;
    // if a character has been printed...
    if (ram[S_POSN] != 33)
    {
      // check available space in line
      if (++column == width)
      {
        fprintf(output, "\n");
        column = 0;
      }
      // output it
      fprintf(output, "%s", table[ram[d_file]]);
      // and make 33 columns available again
      ram[S_POSN] = 33;
    }
    // if a new line has begun...
    if (ram[S_POSN + 1] != 24)
    {
      // we output a new line
      fprintf(output, "\n");
      // zero the column counter
      column = -1;
      // and make 33 columns and 24 lines available again
      ram[S_POSN    ] = 33;
      ram[S_POSN + 1] = 24;
    }
    // executes one z80 instruction
		PC = simz80(PC) & 0xffff;
	}
  
  // all done, close output file and exit
  if (output != stdout)
  {
    fclose(output);
  }
  return 0;
}
