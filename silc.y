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


int main (int argc, char *argv[]) {
	extern FILE * yyin;
	yyin = fopen(argv[1],"r");
	if (yyin) yyparse();
	else {
		yyin = fopen("input.txt","r");
		if (yyin) yyparse();
		else printf("Could not open file\n");
	}
}
%}

%union {
	int number;
	struct Node *node;
}

%token <number> NUMBER ID READ WRITE BREAK EXIT  IF THEN ELSE ENDIF WHILE DO ENDWHILE;

%type <node> exp slist statement;

%right TOKEQUAL
%left '<' '>' 'g' 'l'
%left '+' '-'
%left '*' '/' '%'

%start Program
%%

Program: Init Body EXIT BREAK	{
									fprintf(fp, "HALT");
									fclose(fp);
									exit(0);
								}

Init:	{
			remove("sim.txt");
			fp = fopen("sim.txt", "w");
			fprintf(fp, "START\n");
		}

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
		|	exp '+' exp	{ $$ = MakeNode('+', 'a', $1, $3, 0); }
		|	exp '-' exp	{ $$ = MakeNode('-', 'a', $1, $3, 0); }
		|	exp '*' exp	{ $$ = MakeNode('*', 'a', $1, $3, 0); }
		|	exp '/' exp	{ $$ = MakeNode('/', 'a', $1, $3, 0); }
		|	exp '%' exp	{ $$ = MakeNode('%', 'a', $1, $3, 0); }
		|	exp '>' exp	{ $$ = MakeNode('>', 'r', $1, $3, 0); }
		|	exp '<' exp	{ $$ = MakeNode('<', 'r', $1, $3, 0); }
		|	exp 'g' exp	{ $$ = MakeNode('g', 'r', $1, $3, 0); }
		|	exp 'l' exp	{ $$ = MakeNode('l', 'r', $1, $3, 0); }		
		|	exp TOKEQUAL exp { $$ = MakeNode('=', 'r', $1, $3, 0); }
		|	'(' exp ')' { $$ = $2; }
%%