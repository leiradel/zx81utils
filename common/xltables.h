/*
Translation tables from the ZX-81 character set to ASCII.

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

// translation table for regular output
static const char* table_ascii[256] =
{
  /* 00 */ " ", "#", "#", "#", "#", "#", "#", "#",
  /* 08 */ "#", "#", "#", "\"", "£", "$", ":", "?",
  /* 10 */ "(", ")", ">", "<", "=", "+", "-", "*",
  /* 18 */ "/", ";", ",", ".", "0", "1", "2", "3",
  /* 20 */ "4", "5", "6", "7", "8", "9", "A", "B",
  /* 28 */ "C", "D", "E", "F", "G", "H", "I", "J",
  /* 30 */ "K", "L", "M", "N", "O", "P", "Q", "R",
  /* 38 */ "S", "T", "U", "V", "W", "X", "Y", "Z",
  /* 40 */ "RND", "INKEY$", "PI", "?", "?", "?", "?", "?",
  /* 48 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 50 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 58 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 60 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 68 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 70 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 78 */ "?", "?", "?", "?", "?", "?", "?", "k",
  /* 80 */ "#", "#", "#", "#", "#", "#", "#", "#",
  /* 88 */ "#", "#", "#", "\"", "£", "$", ":", "?",
  /* 90 */ "(", ")", ">", "<", "=", "+", "-", "*",
  /* 98 */ "/", ";", ",", ".", "0", "1", "2", "3",
  /* a0 */ "4", "5", "6", "7", "8", "9", "a", "b",
  /* a8 */ "c", "d", "e", "f", "g", "h", "i", "j",
  /* b0 */ "k", "l", "m", "n", "o", "p", "q", "r",
  /* b8 */ "s", "t", "u", "v", "w", "x", "y", "z",
  /* c0 */ "\"\"", "AT ", "TAB ", "?", "CODE ", "VAL ", "LEN ", "SIN ",
  /* c8 */ "COS ", "TAN ", "ASN ", "ACS ", "ATN ", "LN ", "EXP ", "INT ",
  /* d0 */ "SQR ", "SGN ", "ABS ", "PEEK ", "USR ", "STR$ ", "CHR$ ", "NOT ",
  /* d8 */ "**", " OR ", " AND ", "<=", ">=", "<>", " THEN ", " TO ",
  /* e0 */ " STEP ", " LPRINT ", " LLIST ", " STOP ", " SLOW ", " FAST ", " NEW ", " SCROLL ",
  /* e8 */ " CONT ", " DIM ", " REM ", " FOR ", " GOTO ", " GOSUB ", " INPUT ", " LOAD ",
  /* f0 */ " LIST ", " LET ", " PAUSE ", " NEXT ", " POKE ", " PRINT ", " PLOT ", " RUN ",
  /* f8 */ " SAVE ", " RAND ", " IF ", " CLS ", " UNPLOT ", " CLEAR ", " RETURN ", " COPY ",
};

// translation table to use with the ZX-81.TTF font
static const char* table_zx81[256] =
{
  /* 00 */ " ", "\xc1", "\xc2", "\xc3", "\xc4", "\xc5", "\xc6", "\xc7",
  /* 08 */ "\xb0", "\xb1", "\xb2", "\"", "£", "$", ":", "?",
  /* 10 */ "(", ")", ">", "<", "=", "+", "-", "*",
  /* 18 */ "/", ";", ",", ".", "0", "1", "2", "3",
  /* 20 */ "4", "5", "6", "7", "8", "9", "A", "B",
  /* 28 */ "C", "D", "E", "F", "G", "H", "I", "J",
  /* 30 */ "K", "L", "M", "N", "O", "P", "Q", "R",
  /* 38 */ "S", "T", "U", "V", "W", "X", "Y", "Z",
  /* 40 */ "RND", "INKEY$", "PI", "?", "?", "?", "?", "?",
  /* 48 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 50 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 58 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 60 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 68 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 70 */ "?", "?", "?", "?", "?", "?", "?", "?",
  /* 78 */ "?", "?", "?", "?", "?", "?", "?", "k",
  /* 80 */ "\xcf", "\xce", "\xcd", "\xcc", "\xcb", "\xca", "\xc9", "\xc8",
  /* 88 */ "\xb3", "\xb4", "\xb5", "\xd2", "\xd5", "\xd4", "\xea", "\xef",
  /* 90 */ "\xd8", "\xd9", "\xee", "\xec", "\xed", "\xdb", "\xdd", "\xda",
  /* 98 */ "\xdf", "\xeb", "\xdc", "\xde", "\xe0", "\xe1", "\xe2", "\xe3",
  /* a0 */ "\xe4", "\xe5", "\xe6", "\xe7", "\xe8", "\xe9", "a", "b",
  /* a8 */ "c", "d", "e", "f", "g", "h", "i", "j",
  /* b0 */ "k", "l", "m", "n", "o", "p", "q", "r",
  /* b8 */ "s", "t", "u", "v", "w", "x", "y", "z",
  /* c0 */ "\"\"", "AT ", "TAB ", "?", "CODE ", "VAL ", "LEN ", "SIN ",
  /* c8 */ "COS ", "TAN ", "ASN ", "ACS ", "ATN ", "LN ", "EXP ", "INT ",
  /* d0 */ "SQR ", "SGN ", "ABS ", "PEEK ", "USR ", "STR$ ", "CHR$ ", "NOT ",
  /* d8 */ "**", " OR ", " AND ", "<=", ">=", "<>", " THEN ", " TO ",
  /* e0 */ " STEP ", " LPRINT ", " LLIST ", " STOP ", " SLOW ", " FAST ", " NEW ", " SCROLL ",
  /* e8 */ " CONT ", " DIM ", " REM ", " FOR ", " GOTO ", " GOSUB ", " INPUT ", " LOAD ",
  /* f0 */ " LIST ", " LET ", " PAUSE ", " NEXT ", " POKE ", " PRINT ", " PLOT ", " RUN ",
  /* f8 */ " SAVE ", " RAND ", " IF ", " CLS ", " UNPLOT ", " CLEAR ", " RETURN ", " COPY ",
};  
