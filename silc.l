%{
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "silc.tab.h"
%}

%%
[0-9]+	{ yylval.number = atoi(yytext); return NUMBER; }
;		{ return BREAK; }
"decl"	{ return DECL; }
"enddecl"	{ return ENDDECL; }
"integer"	{ return INTEGER; }
"boolean"	{ return BOOLEAN; }
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
"AND"	{ return AND; }
"OR"	{ return OR; }
"NOT"	{ return NOT; }
"TRUE"	{ return TRUE; }
"FALSE"	{ return FALSE; }

[ \t\n]+ ;
[+\-*/%()=<>] { return (int)yytext[0]; }
[\[\]]	{ return (int)yytext[0]; }
">=" { return 'g'; }
"<=" { return 'l'; }
"=="	{ return TOKEQUAL; }
","	{	return COMMA; }
[a-zA-Z][a-zA-Z0-9]*	{ struct Symbol *p = malloc(sizeof(struct Symbol)); p->Name = strdup(yytext); yylval.Symbol = p; return ID; }
%%