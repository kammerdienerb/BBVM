%{
#include <stdio.h>
#include "y.tab.h"
%}

%option       noyywrap

%%
escape                 return TOK_ESCAPE;
alloc                  return TOK_ALLOC;
load                   return TOK_LOAD;
fload                  return TOK_FLOAD;
store                  return TOK_STORE;
storei                 return TOK_STOREI;
fstore                 return TOK_FSTORE;
fstorei                return TOK_FSTOREI;
getglobal              return TOK_GETGLOBAL;
arg                    return TOK_ARG;
argi                   return TOK_ARGI;
getarg                 return TOK_GETARG;

br                     return TOK_BR;
brc                    return TOK_BRC;
call                   return TOK_CALL;
calli                  return TOK_CALLI;
fficall                return TOK_FFI_CALL;
vret                   return TOK_VRET;
ret                    return TOK_RET;
reti                   return TOK_RETI;

lss					   return TOK_LSS;
lssi                   return TOK_LSSI;
leq                    return TOK_LEQ;
leqi                   return TOK_LEQI;
gtr                    return TOK_GTR;
gtri                   return TOK_GTRI;
geq                    return TOK_GEQ;
geqi                   return TOK_GEQI;
equ                    return TOK_EQU;
equi                   return TOK_EQUI;
neq                    return TOK_NEQ;
neqi                   return TOK_NEQI;
add                    return TOK_ADD;
addi                   return TOK_ADDI;
sub                    return TOK_SUB;
subi                   return TOK_SUBI;
mul                    return TOK_MUL;
muli                   return TOK_MULI;
div                    return TOK_DIV;
divi                   return TOK_DIVI;
mod                    return TOK_MOD;
modi                   return TOK_MODI;
and                    return TOK_AND;
andi                   return TOK_ANDI;
or                     return TOK_OR;
ori                    return TOK_ORI;
flss                   return TOK_FLSS;
flssi                  return TOK_FLSSI;
fleq                   return TOK_FLEQ;
fleqi                  return TOK_FLEQI;
fgtr                   return TOK_FGTR;
fgtri                  return TOK_FGTRI;
fgeq                   return TOK_FGEQ;
fgeqi                  return TOK_FGEQI;
fequ                   return TOK_FEQU;
fequi                  return TOK_FEQUI;
fneq                   return TOK_FNEQ;
fneqi                  return TOK_FNEQI;
fadd                   return TOK_FADD;
faddi                  return TOK_FADDI;
fsub                   return TOK_FSUB;
fsubi                  return TOK_FSUBI;
fmul                   return TOK_FMUL;
fmuli                  return TOK_FMULI;
fdiv                   return TOK_FDIV;
fdivi                  return TOK_FDIVI;
print                  return TOK_PRINT;
fprint                 return TOK_FPRINT;


"."                    return TOK_DOT;
","                    return TOK_COMMA;
"%"                    return TOK_PERCENT;
":"                    return TOK_COLON;
"-"                    return TOK_MINUS;
"="                    return TOK_ASSIGN;

[0-9]+                 return TOK_DEC;
-?[0-9]+\.[0-9]+       return TOK_FLOAT_LITERAL;

[a-zA-Z_][a-zA-Z0-9_]* return TOK_ID;

[ \t\r\n]	;

