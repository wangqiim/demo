%{
#include "calc.tab.h"
void yyerror (char *s);
int yylex();
%}
%%
[0-9]+                 {yylval.num = atoi(yytext); return number;}
[ \t\n]                ;
[-+*/a;]               {return yytext[0];}
.                      {ECHO; yyerror ("unexpected character");}

%%
int yywrap (void) {return 1;}

