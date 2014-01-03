%{
#include <stdio.h>
#include "tree.h"
#include "silc.tab.h"

%}

%%
[0-9]+	{ yylval.number = atoi(yytext); return NUMBER; }
[a-z]	{ yylval.number = (int)yytext[0]; return ID; }
;		{ return BREAK; }
"read"	{ return READ; }
"write"	{ return WRITE; }
"exit"	{ return EXIT; }
"if"	{ return IF; }
"then"	{ return THEN; }
"endif"	{ return ENDIF; }
[ \t\n]+ ;
[+\-*/%()=<>] { return (int)yytext[0]; }
">=" { return 'g'; }
"<=" { return 'l'; }
"=="	{ return TOKEQUAL; }
%%