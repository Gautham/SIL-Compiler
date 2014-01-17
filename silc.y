%{
#include <stdio.h>
#include "definitions.h"
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
	struct Symbol *Symbol;
	struct Node *node;
}

%token <number> NUMBER READ WRITE BREAK EXIT;
%token <number> IF THEN ELSE ENDIF WHILE DO ENDWHILE;
%token <number> DECL ENDDECL INTEGER BOOLEAN COMMA;
%token <number> AND OR NOT;
%token <number> TRUE FALSE;


%token <Symbol> ID;
%type <node> exp slist statement;
%type <Symbol> NewScope InitNewScope;

%left NOT
%left AND OR
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

Body: NewScope slist { TopScope = $1; Parse($2); }

NewScope:	InitNewScope DeclarationBody { $$ = $1; }

InitNewScope:	{ $$ = NewScope(); }

EndNewScope: { TopScope = TopScope->parent; }

DeclarationBody:	DECL Declarations ENDDECL { ; }
					|	;

Declarations:	Declarations INTEGER TypeInt DEFLIST BREAK
				|	Declarations BOOLEAN TypeBool DEFLIST BREAK
				|	;

TypeInt:	{ DeclType = 1; }

TypeBool:	{ DeclType = 2; }



DEFLIST:	DEFLIST COMMA ID { InstallVariable($3, 1, 0); }
			|	ID { InstallVariable($1, 1, 0); }
			|	DEFLIST COMMA ID '[' exp ']' { InstallVariable($3, 0, $5); }
			|	ID '[' exp ']' { InstallVariable($1, 0, $3); }

slist:	slist statement BREAK { $$ = MakeNode(0, 'S', $1, $2, 0, 0, 0); }
		|	statement BREAK { $$ = $1; }

statement:	WRITE '(' exp ')' { $$ = MakeNode(0, 'W', $3, 0, 0, 0, 0); }
			|	READ '(' ID ')' { $$ = MakeNode(0, 'R', 0, 0, 0, $3, 0); }
			|	READ '(' ID '[' exp ']' ')' { $$ = MakeNode(0, 'R', $5, 0, 0, $3, 0); }
			|	ID '=' exp { $$ = MakeNode(0, 'A', 0, $3, 0, $1, 0); }
			|	ID '[' exp ']' '=' exp { $$ = MakeNode(0, 'A', $3, $6, 0, $1, 0); }			
			|	IF '(' exp ')' THEN NewScope slist EndNewScope ENDIF { $$ = MakeNode(0, 'C', $3, $7, 0, $6, 0); }
			|	IF '(' exp ')' THEN NewScope slist EndNewScope ELSE NewScope slist EndNewScope ENDIF { $$ = MakeNode(0, 'C', $3, $7, $11, $6, $10); }
			|	WHILE '(' exp ')' DO NewScope slist EndNewScope ENDWHILE { $$ = MakeNode(0, 'L', $3, $7, 0, $6, 0); }

exp:	NUMBER	{ $$ = MakeNode($1, 'i', 0, 0, 0, 0, 0); }
		|	ID	{ $$ = MakeNode(0, 'v', 0, 0, 0, $1, 0); }
		|	ID '[' exp ']'	{ $$ = MakeNode(0, 'v', $3, 0, 0, $1, 0); }		
		|	exp '+' exp	{ $$ = MakeNode('+', 'a', $1, $3, 0, 0, 0); }
		|	exp '-' exp	{ $$ = MakeNode('-', 'a', $1, $3, 0, 0, 0); }
		|	exp '*' exp	{ $$ = MakeNode('*', 'a', $1, $3, 0, 0, 0); }
		|	exp '/' exp	{ $$ = MakeNode('/', 'a', $1, $3, 0, 0, 0); }
		|	exp '%' exp	{ $$ = MakeNode('%', 'a', $1, $3, 0, 0, 0); }
		|	exp '>' exp	{ $$ = MakeNode('>', 'r', $1, $3, 0, 0, 0); }
		|	exp '<' exp	{ $$ = MakeNode('<', 'r', $1, $3, 0, 0, 0); }
		|	exp 'g' exp	{ $$ = MakeNode('g', 'r', $1, $3, 0, 0, 0); }
		|	exp 'l' exp	{ $$ = MakeNode('l', 'r', $1, $3, 0, 0, 0); }		
		|	exp TOKEQUAL exp { $$ = MakeNode('=', 'r', $1, $3, 0, 0, 0); }
		|	exp AND exp	{ $$ = MakeNode('a', 'l', $1, $3, 0, 0, 0); }		
		|	exp OR exp	{ $$ = MakeNode('o', 'l', $1, $3, 0, 0, 0); }		
		|	NOT exp	{ $$ = MakeNode('n', 'l', $2, 0, 0, 0, 0); }
		|	'(' exp ')' { $$ = $2; }
		|	TRUE	{ $$ = MakeNode(1, 'b', 0, 0, 0, 0, 0); }
		|	FALSE	{ $$ = MakeNode(0, 'b', 0, 0, 0, 0, 0); }
%%