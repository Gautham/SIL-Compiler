%{
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "silc.tab.h"
%}

%%
[0-9]+	{ yylval.number = atoi(yytext); return NUMBER; }
;		{ return BREAK; }
"decl"	{ return DECLARE; }
"read"	{ return READ; }
"write"	{ return WRITE; }
"exit"	{ return EXIT; }
"if"	{ return IF; }
"then"	{ return THEN; }
"else"	{ return ELSE; }
"endif"	{ return ENDIF; }
"while"	{ return WHILE; }
"do"	{ return DO; }
"endwhile"	{ return ENDWHILE; }
[ \t\n]+ ;
[+\-*/%()=<>] { return (int)yytext[0]; }
[\[\]]	{ return (int)yytext[0]; }
">=" { return 'g'; }
"<=" { return 'l'; }
"=="	{ return TOKEQUAL; }
[a-zA-Z][a-zA-Z0-9]*	{ struct gSymbol *p = malloc(sizeof(struct gSymbol)); p->Name = strdup(yytext); yylval.gSym = p; return ID; }
%%