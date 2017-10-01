/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOK_ENTRY = 258,
     TOK_ESCAPE = 259,
     TOK_ALLOC = 260,
     TOK_LOAD = 261,
     TOK_FLOAD = 262,
     TOK_STORE = 263,
     TOK_STOREI = 264,
     TOK_FSTORE = 265,
     TOK_FSTOREI = 266,
     TOK_GETGLOBAL = 267,
     TOK_ARG = 268,
     TOK_ARGI = 269,
     TOK_GETARG = 270,
     TOK_BR = 271,
     TOK_BRC = 272,
     TOK_CALL = 273,
     TOK_CALLI = 274,
     TOK_FFI_CALL = 275,
     TOK_VRET = 276,
     TOK_RET = 277,
     TOK_RETI = 278,
     TOK_LSSI = 279,
     TOK_LEQ = 280,
     TOK_LEQI = 281,
     TOK_GTR = 282,
     TOK_GTRI = 283,
     TOK_GEQ = 284,
     TOK_GEQI = 285,
     TOK_EQU = 286,
     TOK_EQUI = 287,
     TOK_NEQ = 288,
     TOK_NEQI = 289,
     TOK_ADD = 290,
     TOK_ADDI = 291,
     TOK_SUB = 292,
     TOK_SUBI = 293,
     TOK_MUL = 294,
     TOK_MULI = 295,
     TOK_DIV = 296,
     TOK_DIVI = 297,
     TOK_MOD = 298,
     TOK_MODI = 299,
     TOK_AND = 300,
     TOK_ANDI = 301,
     TOK_OR = 302,
     TOK_ORI = 303,
     TOK_FLSS = 304,
     TOK_FLSSI = 305,
     TOK_FLEQ = 306,
     TOK_FLEQI = 307,
     TOK_FGTR = 308,
     TOK_FGTRI = 309,
     TOK_FGEQ = 310,
     TOK_FGEQI = 311,
     TOK_FEQU = 312,
     TOK_FEQUI = 313,
     TOK_FNEQ = 314,
     TOK_FNEQI = 315,
     TOK_FADD = 316,
     TOK_FADDI = 317,
     TOK_FSUB = 318,
     TOK_FSUBI = 319,
     TOK_FMUL = 320,
     TOK_FMULI = 321,
     TOK_FDIV = 322,
     TOK_FDIVI = 323,
     TOK_PRINT = 324,
     TOK_FPRINT = 325,
     TOK_DOT = 326,
     TOK_COMMA = 327,
     TOK_PERCENT = 328,
     TOK_COLON = 329,
     TOK_MINUS = 330,
     TOK_ASSIGN = 331,
     TOK_DEC = 332,
     TOK_FLOAT_LITERAL = 333,
     TOK_ID = 334
   };
#endif
/* Tokens.  */
#define TOK_ENTRY 258
#define TOK_ESCAPE 259
#define TOK_ALLOC 260
#define TOK_LOAD 261
#define TOK_FLOAD 262
#define TOK_STORE 263
#define TOK_STOREI 264
#define TOK_FSTORE 265
#define TOK_FSTOREI 266
#define TOK_GETGLOBAL 267
#define TOK_ARG 268
#define TOK_ARGI 269
#define TOK_GETARG 270
#define TOK_BR 271
#define TOK_BRC 272
#define TOK_CALL 273
#define TOK_CALLI 274
#define TOK_FFI_CALL 275
#define TOK_VRET 276
#define TOK_RET 277
#define TOK_RETI 278
#define TOK_LSSI 279
#define TOK_LEQ 280
#define TOK_LEQI 281
#define TOK_GTR 282
#define TOK_GTRI 283
#define TOK_GEQ 284
#define TOK_GEQI 285
#define TOK_EQU 286
#define TOK_EQUI 287
#define TOK_NEQ 288
#define TOK_NEQI 289
#define TOK_ADD 290
#define TOK_ADDI 291
#define TOK_SUB 292
#define TOK_SUBI 293
#define TOK_MUL 294
#define TOK_MULI 295
#define TOK_DIV 296
#define TOK_DIVI 297
#define TOK_MOD 298
#define TOK_MODI 299
#define TOK_AND 300
#define TOK_ANDI 301
#define TOK_OR 302
#define TOK_ORI 303
#define TOK_FLSS 304
#define TOK_FLSSI 305
#define TOK_FLEQ 306
#define TOK_FLEQI 307
#define TOK_FGTR 308
#define TOK_FGTRI 309
#define TOK_FGEQ 310
#define TOK_FGEQI 311
#define TOK_FEQU 312
#define TOK_FEQUI 313
#define TOK_FNEQ 314
#define TOK_FNEQI 315
#define TOK_FADD 316
#define TOK_FADDI 317
#define TOK_FSUB 318
#define TOK_FSUBI 319
#define TOK_FMUL 320
#define TOK_FMULI 321
#define TOK_FDIV 322
#define TOK_FDIVI 323
#define TOK_PRINT 324
#define TOK_FPRINT 325
#define TOK_DOT 326
#define TOK_COMMA 327
#define TOK_PERCENT 328
#define TOK_COLON 329
#define TOK_MINUS 330
#define TOK_ASSIGN 331
#define TOK_DEC 332
#define TOK_FLOAT_LITERAL 333
#define TOK_ID 334




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

