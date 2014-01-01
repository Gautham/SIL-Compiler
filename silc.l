%{
#include <stdio.h>
#include "tree.h"
#include "silc.tab.h"

%}

%%
[0-9]+	{ yylval.number = atoi(yytext); return NUMBER; }
[a-z]	{ yylval.number = (int)yytext[0]; return ID; }
;		{ return BREAK; }
"read"	{ return TOKREAD; }
"write"	{ return TOKWRITE; }
"exit"	{ return TOKEXIT; }
[ \t\n]+ ;
[+\-*/%()=<>] { return (int)yytext[0]; }
"=="	{ return TOKEQUAL; }
%%