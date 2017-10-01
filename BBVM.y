%{
/*
 * BBVM.y
 */

#include <stdio.h>

int yylex();
void yyerror(const char *s);
%}

%token TOK_ENTRY
%token TOK_ESCAPE
%token TOK_ALLOC
%token TOK_LOAD
%token TOK_FLOAD
%token TOK_STORE
%token TOK_STOREI
%token TOK_FSTORE
%token TOK_FSTOREI
%token TOK_GETGLOBAL
%token TOK_ARG
%token TOK_ARGI
%token TOK_GETARG
%token TOK_BR
%token TOK_BRC
%token TOK_CALL
%token TOK_CALLI
%token TOK_FFI_CALL
%token TOK_VRET
%token TOK_RET
%token TOK_RETI
%token TOK_LSSI
%token TOK_LEQ
%token TOK_LEQI
%token TOK_GTR
%token TOK_GTRI
%token TOK_GEQ
%token TOK_GEQI
%token TOK_EQU
%token TOK_EQUI
%token TOK_NEQ
%token TOK_NEQI
%token TOK_ADD
%token TOK_ADDI
%token TOK_SUB
%token TOK_SUBI
%token TOK_MUL
%token TOK_MULI
%token TOK_DIV
%token TOK_DIVI
%token TOK_MOD
%token TOK_MODI
%token TOK_AND
%token TOK_ANDI
%token TOK_OR
%token TOK_ORI
%token TOK_FLSS
%token TOK_FLSSI
%token TOK_FLEQ
%token TOK_FLEQI
%token TOK_FGTR
%token TOK_FGTRI
%token TOK_FGEQ
%token TOK_FGEQI
%token TOK_FEQU
%token TOK_FEQUI
%token TOK_FNEQ
%token TOK_FNEQI
%token TOK_FADD
%token TOK_FADDI
%token TOK_FSUB
%token TOK_FSUBI
%token TOK_FMUL
%token TOK_FMULI
%token TOK_FDIV
%token TOK_FDIVI
%token TOK_PRINT
%token TOK_FPRINT
%token TOK_DOT
%token TOK_COMMA
%token TOK_PERCENT
%token TOK_COLON
%token TOK_MINUS
%token TOK_ASSIGN
%token TOK_DEC
%token TOK_FLOAT_LITERAL
%token TOK_ID

%%
basic_block: TOK_ID TOK_COLON zero_or_more_instructions
		   ;

zero_or_more_instructions: /* none */
						 | zero_or_more_instructions instruction
						 ;

instruction: TOK_ENTRY
		   | TOK_ESCAPE
		   | TOK_ALLOC TOK_DEC
		   | ssa_ref TOK_ASSIGN TOK_LOAD ssa_ref
		   | ssa_ref TOK_ASSIGN TOK_FLOAD ssa_ref
		   | TOK_STORE ssa_ref TOK_COMMA ssa_ref 
		   | TOK_STOREI ssa_ref TOK_COMMA signed_dec
		   | TOK_FSTORE ssa_ref TOK_COMMA ssa_ref 
		   | TOK_FSTOREI ssa_ref TOK_COMMA TOK_FLOAT_LITERAL
		   | TOK_GETGLOBAL TOK_DEC
		   | TOK_ARG ssa_ref
		   | TOK_ARGI signed_dec
		   | ssa_ref TOK_ASSIGN TOK_GETARG TOK_DEC
		   | TOK_BR bb_ref
		   | TOK_BRC ssa_ref bb_ref
		   | ssa_ref TOK_ASSIGN TOK_CALL bb_ref
		   | ssa_ref TOK_ASSIGN TOK_FFI_CALL bb_ref
		   ;

ssa_ref: TOK_PERCENT TOK_DEC
	   ;

bb_ref: TOK_PERCENT TOK_ID
	  ;

signed_dec:	TOK_MINUS TOK_DEC
		  | TOK_DEC
		  ;

%%

void yyerror(const char * s) {
   fprintf(stderr, "%s\n", s);
}

