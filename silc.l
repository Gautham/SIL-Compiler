%{
#include <stdio.h>
#include "tree.h"
#include "silc.tab.h"

%}

%%
[0-9]+	{ yylval.number = atoi(yytext); return NUMBER; }
[ \t]+ ;
[+\-*/()\n] { return (int)yytext[0]; }
%%