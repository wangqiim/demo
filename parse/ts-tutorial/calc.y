%{
void yyerror (char *s);
int yylex();
#include <stdio.h>     /* C declarations used in actions */
#include <stdlib.h>
#include <ctype.h>
%}

%union {int num; char id;}         /* Yacc definitions */
%start line
%token <num> number
%type <num> line exp term

%left 'a' '+'
%left '*'
%left ptm

%%

/* descriptions of expected inputs     corresponding actions (in C) */

line : exp ';'              {printf("Printing %d\n", $1);}
     | line exp ';'         {printf("Printing %d\n", $2);}
     ;
    

exp : exp '+' exp            {$$ = $1 + $3;}
    | exp 'a' exp            {$$ = $1 - $3;}
    | exp '*' exp            {$$ = $1 * $3;}
    | 'a' exp %prec ptm      {$$ = $2 * $2;}
    | term                   {$$ = $1;}
    ; 
    
term : number                {$$ = $1;}
     ;

%%                     /* C code */
int main (void) {
  /* init symbol table */
  return yyparse ( );
}

void yyerror (char *s) {
  fprintf (stderr, "%s\n", s);
} 

