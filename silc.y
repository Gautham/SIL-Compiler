%{
#include <stdio.h>
#include <stdlib.h>
#include "generator.h"

void yyerror(const char *str)
{
	fprintf(stderr,"Error at Line %d: %s\n", LineCount, str);
	exit(0);
}


%}

%union
{
	int number;
	struct Symbol *Symbol;
	struct Node *node;
}

%token <number> NUMBER
%token <number> MAIN READ WRITE COMMA BREAK RETURN EXIT;
%token <number> IF THEN ELSE ENDIF;
%token <number> WHILE DO ENDWHILE;
%token <number> DECL ENDDECL INTEGER BOOLEAN;
%token <number> AND OR NOT;
%token <number> BOOL;
%token <Symbol> ID;


%type <number> SetType FunctionType;
%type <node> exp slist statement Variable FunctionCall ActualParameters;
%type <Symbol> GlobalDeclarations NewScope InitNewScope StaticScope InitializeStaticScope StackScope InitializeStackScope;

%left NOT AND OR
%right EQ
%left '<' '>' GE LE
%left '+' '-'
%left '*' '/' '%'

%start Program
%%

Program: GlobalDeclarations FunctionDefinitions MainFunction


GlobalDeclarations:	StaticScope
FunctionDefinitions:	FunctionDefinitions Function
						| ;
MainFunction:	INTEGER MAIN '{' StaticScope slist EXIT BREAK '}' { MainFunction = $5; }

StaticScope: InitializeStaticScope Declarations { $$ = $1; }
StackScope: InitializeStackScope Declarations
			{
				$1->parent = Arg;
				$$ = $1;
			}

InitializeStaticScope:	{ $$ = NewScope('S'); }
InitializeStackScope:	{ $$ = NewScope('R'); }


Function:	FunctionType ID '(' FormalParameters ')' '{' StackScope slist RETURN exp BREAK EndScope '}'
				{
					$2->Type = $1;
					InstallFunction($2, $7);
					GenerateFunction($8, $2, $7, $10);
				}

FunctionType:	INTEGER		{ $$ = 1; }
				|	BOOLEAN	{ $$ = 0; }


FormalParameters:	InitializeNewParameterList FormalParameterTypeList;

InitializeNewParameterList:	{ NewParameterList(); }

FormalParameterTypeList:	FormalParameterTypeList INTEGER TypeInt FormalParameterList BREAK
							|	FormalParameterTypeList BOOLEAN TypeBool FormalParameterList BREAK
							|	;

FormalParameterList:	FormalParameterList COMMA ID { AddParam($3, 1); }
						|	ID { AddParam($1, 1); }

Arguements:	InitializeNewArguementList	ArguementSet ;

InitializeNewArguementList:	{ NewArguementList(); }

ArguementSet:	ArguementSet INTEGER TypeInt ArguementList BREAK
			|	ArguementSet BOOLEAN TypeBool ArguementList BREAK
			|	;

ArguementList:	ArguementList COMMA ID { InstallArguement($3, 1); }
			|	ID { InstallArguement($1, 1); }


NewScope:	InitNewScope Declarations { $$ = $1; }

InitNewScope:	{ $$ = NewScope(TopScope->Type); }

EndScope:	{
				if (TopScope->Type == 'R' && TopScope->parent->Type != 'A') TopScope->parent->Size += TopScope->Size;
				TopScope = TopScope->parent;
			}


Declarations:	DECL DeclarationBody ENDDECL { ; }
				|	;

DeclarationBody:	DeclarationBody INTEGER TypeInt DEFLIST BREAK
					|	DeclarationBody BOOLEAN TypeBool DEFLIST BREAK
					|	INTEGER TypeInt DEFLIST BREAK
					|	BOOLEAN TypeBool DEFLIST BREAK

TypeInt:	{ DeclType = 1; }
TypeBool:	{ DeclType = 0; }

DEFLIST:	DEFLIST COMMA ID { InstallVariable($3, 1); }
			|	ID SetType '(' Arguements ')'
				{
					$1->Type = $2;
					DeclareFunction($1, Arg);
					TopScope = TopScope->parent;
				}
			|	ID { InstallVariable($1, 1); }
			|	DEFLIST COMMA ID '[' NUMBER ']' { InstallVariable($3, $5); }
			|	ID '[' NUMBER ']' { InstallVariable($1, $3); }
			|	DEFLIST COMMA ID SetType '(' Arguements ')'
				{
					$3->Type = $4;
					DeclareFunction($3, Arg);
					TopScope = TopScope->parent;
				}
SetType:	{	$$ = DeclType; }


slist:	slist statement BREAK { $$ = MakeNode(0, 'S', $1, $2, 0, 0, 0); }
		|	statement BREAK { $$ = $1; }

statement:	WRITE '(' exp ')' { $$ = MakeNode(0, 'W', $3, 0, 0, 0, 0); }
			|	READ '(' ID ')' { $$ = MakeNode(0, 'R', 0, 0, 0, $3, 0); }
			|	READ '(' ID '[' exp ']' ')' { $$ = MakeNode(0, 'R', $5, 0, 0, $3, 0); }
			|	ID '=' exp { $$ = MakeNode(0, 'A', 0, $3, 0, $1, 0); }
			|	ID '[' exp ']' '=' exp { $$ = MakeNode(0, 'A', $3, $6, 0, $1, 0); }
			|	IF '(' exp ')' THEN NewScope slist EndScope ENDIF { $$ = MakeNode(0, 'C', $3, $7, 0, $6, 0); }
			|	IF '(' exp ')' THEN NewScope slist EndScope ELSE NewScope slist EndScope ENDIF { $$ = MakeNode(0, 'C', $3, $7, $11, $6, $10); }
			|	WHILE '(' exp ')' DO NewScope slist EndScope ENDWHILE { $$ = MakeNode(0, 'L', $3, $7, 0, $6, 0); }

ActualParameters:	ActualParameters COMMA exp
					{
						struct Node *F = MakeNode(0, 'P', $3, 0, 0, 0, 0);
						$1->t2->t3 = F;
						$1->t2 = F;
						$$ = $1;
					}
					|	exp
						{
							$$ = MakeNode(0, 'P', $1, 0, 0, 0, 0);
							$$->t2 = $$;
						}

exp:	NUMBER	{ $$ = MakeNode($1, 'i', 0, 0, 0, 0, 0); }
		|	FunctionCall { $$ = $1; }
		|	Variable { $$ = $1; }
		|	exp '+' exp	{ $$ = MakeNode('+', 'a', $1, $3, 0, 0, 0); }
		|	exp '-' exp	{ $$ = MakeNode('-', 'a', $1, $3, 0, 0, 0); }
		|	exp '*' exp	{ $$ = MakeNode('*', 'a', $1, $3, 0, 0, 0); }
		|	exp '/' exp	{ $$ = MakeNode('/', 'a', $1, $3, 0, 0, 0); }
		|	exp '%' exp	{ $$ = MakeNode('%', 'a', $1, $3, 0, 0, 0); }
		|	exp '>' exp	{ $$ = MakeNode('>', 'r', $1, $3, 0, 0, 0); }
		|	exp '<' exp	{ $$ = MakeNode('<', 'r', $1, $3, 0, 0, 0); }
		|	exp GE exp	{ $$ = MakeNode('g', 'r', $1, $3, 0, 0, 0); }
		|	exp LE exp	{ $$ = MakeNode('l', 'r', $1, $3, 0, 0, 0); }
		|	exp EQ exp { $$ = MakeNode('=', 'r', $1, $3, 0, 0, 0); }
		|	exp AND exp	{ $$ = MakeNode('a', 'l', $1, $3, 0, 0, 0); }
		|	exp OR exp	{ $$ = MakeNode('o', 'l', $1, $3, 0, 0, 0); }
		|	NOT exp	{ $$ = MakeNode('n', 'l', $2, 0, 0, 0, 0); }
		|	'(' exp ')' { $$ = $2; }
		|	BOOL	{ $$ = MakeNode($1, 'b', 0, 0, 0, 0, 0); }

Variable:	ID	{ $$ = MakeNode(0, 'v', 0, 0, 0, $1, 0); }
			|	ID '[' exp ']' { $$ = MakeNode(0, 'v', $3, 0, 0, $1, 0); }

FunctionCall:	ID '(' ')' { $$ = MakeNode(0, 'F', 0, 0, 0, 0, $1); }
				|	ID '(' ActualParameters ')' { $$ = MakeNode(0, 'F', $3, 0, 0, 0, $1); }


%%

int yywrap()
{
	return 1;
}

int main (int argc, char *argv[])
{
	remove("sim.txt");
	fp = fopen("sim.txt", "w");
	fprintf(fp, "START\n");
	fprintf(fp, "JMP MAIN\n");
	extern FILE * yyin;
	yyin = fopen(argv[1],"r");
	if (yyin) yyparse();
	else {
		yyin = fopen("input.txt","r");
		if (yyin) yyparse();
		else printf("Could not open file\n");
	}
	fprintf(fp, "MAIN:\n");
	fprintf(fp, "MOV SP, %d\n", SP);
	Generate(MainFunction);
	fprintf(fp, "HALT");
	fclose(fp);
	exit(0);
}