%{
#include <stdio.h>
#include "tree.h"
#include "main.h"
#include <stdlib.h>

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
	return 1;
}


main()
{
	yyparse();
}
%}

%union {
	int number;
	struct Node *node;
}

%token <number> NUMBER ID READ WRITE BREAK EXIT TOKEQUAL IF THEN ELSE ENDIF WHILE DO ENDWHILE;

%type <node> exp Body slist statement;

%right TOKEQUAL
%left '<' '>' 'g' 'l'
%left '+' '-'
%left '*' '/' '%'

%start Program
%%

Program: Body EXIT BREAK { exit(0); }

Body: slist { Evaluate($1); }

slist:	slist statement BREAK { $$ = MakeNode(0, 'S', $2, $1, 0); }
		|	statement BREAK { $$ = $1; }

statement:	WRITE '(' exp ')' { $$ = MakeNode(0, 'W', $3, 0, 0); }
			|	READ '(' ID ')' { $$ = MakeNode($3, 'R', 0, 0, 0); }
			|	ID '=' exp { $$ = MakeNode($1, 'A', $3, 0, 0); }
			|	IF '(' exp ')' THEN slist ENDIF { $$ = MakeNode(0, 'C', $3, $6, 0); }
			|	IF '(' exp ')' THEN slist ELSE slist ENDIF { $$ = MakeNode(0, 'C', $3, $6, $8); }
			|	WHILE '(' exp ')' DO slist ENDWHILE { $$ = MakeNode(0, 'L', $3, $6, 0); }

exp:	NUMBER	{ $$ = MakeNode($1, 'i', 0, 0, 0); }
		|	ID	{ $$ = MakeNode($1, 'v', 0, 0, 0); }
		|	exp '+' exp	{ $$ = MakeNode(0, '+', $1, $3, 0); }
		|	exp '-' exp	{ $$ = MakeNode(0, '-', $1, $3, 0); }
		|	exp '*' exp	{ $$ = MakeNode(0, '*', $1, $3, 0); }
		|	exp '/' exp	{ $$ = MakeNode(0, '/', $1, $3, 0); }
		|	exp '%' exp	{ $$ = MakeNode(0, '%', $1, $3, 0); }
		|	exp '>' exp	{ $$ = MakeNode(0, '>', $1, $3, 0); }
		|	exp '<' exp	{ $$ = MakeNode(0, '<', $1, $3, 0); }
		|	exp 'g' exp	{ $$ = MakeNode(0, 'g', $1, $3, 0); }
		|	exp 'l' exp	{ $$ = MakeNode(0, 'l', $1, $3, 0); }		
		|	exp TOKEQUAL exp { $$ = MakeNode(0, '=', $1, $3, 0); }
		|	'(' exp ')' { $$ = $2; }
%%