%{
#include <stdio.h>
#include "silc.tab.h"
%}

%%
[0-9]+	{ yylval = atoi(yytext); return NUMBER; }
[ \t]+ ;
[+\-*/()\n] { return (int)yytext[0]; }
%%