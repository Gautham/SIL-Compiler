%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"


void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
	return 1;
}

int main (int argc, char *argv[]) {
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
	Parse(MainFunction);
	fprintf(fp, "HALT");
	fclose(fp);
	exit(0);
}
%}

%union {
	int number;
	struct Symbol *Symbol;
	struct Node *node;
	struct Parameters *parameter;
}

%token <number> NUMBER
%token <number> MAIN READ WRITE COMMA BREAK RETURN EXIT;
%token <number> IF THEN ELSE ENDIF;
%token <number> WHILE DO ENDWHILE;
%token <number> DECL ENDDECL INTEGER BOOLEAN;
%token <number> AND OR NOT;
%token <number> TRUE FALSE;
%token <Symbol> ID;


%type <number> SetType FunctType;
%type <node> exp slist statement;
%type <parameter> Parameters
%type <Symbol> GlobalDeclarations NewScope InitNewScope StaticScope InitStaticScope StackScope InitStackScope;

%left NOT
%left AND OR
%right TOKEQUAL
%left '<' '>' 'g' 'l'
%left '+' '-'
%left '*' '/' '%'

%start Program
%%

Program: GlobalDeclarations
GlobalDeclarations:	StaticScope FunctionList

StaticScope: InitStaticScope DeclarationBody { $$ = $1; }
InitStaticScope:	{ $$ = NewScope('S'); }


StackScope: InitStackScope DeclarationBody { $1->parent = Arg; $$ = $1; }
InitStackScope:	{ $$ = NewScope('R'); }

FunctionList:	FunctionList Function
				| Function;

Function:	MainFunction
			|	FunctType ID '(' FormalParameters ')' '{' StackScope slist RETURN exp BREAK '}' EndScope	{
																				$2->Type = $1;
																				InstallFunction($2, $7);
																				ParseFunction($8, $2, $7, $10);
																		}

FunctType:	INTEGER		{ $$ = 3; }
			|	BOOLEAN	{ $$ = 4; }

FormalParameters:	InitNewParamList FormalParams;

InitNewParamList:	{ NewParamList(); }

FormalParams:	FormalParams INTEGER TypeInt FormalParamList BREAK
				|	FormalParams BOOLEAN TypeBool FormalParamList BREAK
				|	;

FormalParamList:	FormalParamList COMMA ID { AddParam($3, 1); }
				|	ID { AddParam($1, 1); }
				|	FormalParamList COMMA ID '[' NUMBER ']' { AddParam($3, $5); }
				|	ID '[' NUMBER ']' { AddParam($1, $3); }

Arguements:	InitNewArgList	ArguementSet ;

InitNewArgList:	{ NewArgList(); }

ArguementSet:	ArguementSet INTEGER TypeInt ArgList BREAK
			|	ArguementSet BOOLEAN TypeBool ArgList BREAK
			|	;

ArgList:	ArgList COMMA ID { InstallArguement($3, 1); }
			|	ID { InstallArguement($1, 1); }
			|	ArgList COMMA ID '[' NUMBER ']' { InstallArguement($3, $5); }
			|	ID '[' NUMBER ']' { InstallArguement($1, $3); }


MainFunction: INTEGER MAIN '{' StaticScope slist EXIT BREAK '}'	{
																	TopScope = $4;
																	TopScope->Binding = -1;
																	fprintf(fp, "MAIN:\n");
																	fprintf(fp, "MOV SP, %d\n", SP);
																	FlagMain = 1;
																	MainFunction = $5;
																}
NewScope:	InitNewScope DeclarationBody { $$ = $1; }

InitNewScope:	{ $$ = NewScope(TopScope->Type); }

EndScope:	{
				if (TopScope->Type == 'R' && TopScope->parent->Type != 'A') TopScope->parent->Size += TopScope->Size;
				TopScope = TopScope->parent;
			}

DeclarationBody:	DECL Declarations ENDDECL { ; }
					|	;

Declarations:	Declarations INTEGER TypeInt DEFLIST BREAK
				|	Declarations BOOLEAN TypeBool DEFLIST BREAK
				|	INTEGER TypeInt DEFLIST BREAK
				|	BOOLEAN TypeBool DEFLIST BREAK

TypeInt:	{ DeclType = 1; }

TypeBool:	{ DeclType = 2; }

DEFLIST:	DEFLIST COMMA ID { InstallVariable($3, 1); }
			|	ID SetType '(' Arguements ')' { $1->Type = $2; DeclareFunction($1, Arg); TopScope = TopScope->parent; }
			|	ID { InstallVariable($1, 1); }
			|	DEFLIST COMMA ID '[' NUMBER ']' { InstallVariable($3, $5); }
			|	ID '[' NUMBER ']' { InstallVariable($1, $3); }
			|	DEFLIST COMMA ID SetType '(' Arguements ')' { $3->Type = $4; DeclareFunction($3, Arg); TopScope = TopScope->parent; }

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

Parameters:	Parameters COMMA exp	{
										struct Parameters * F = $1;
										while (F->next) F = F->next;
										F->next = malloc(sizeof(struct Parameters));
										F->next->t = $3;
										F->next->next = 0;
										$$ = $1;
									} 
			|	exp		{
							$$ = malloc(sizeof(struct Parameters));
							$$->t = $1;
							$$->next = 0;
						}

exp:	NUMBER	{ $$ = MakeNode($1, 'i', 0, 0, 0, 0, 0); }
		|	ID '(' ')' { Para = 0; $$ = MakeNode(0, 'F', 0, 0, 0, 0, $1); }
		|	ID '(' Parameters BREAK ')' { Para = $3; $$ = MakeNode(0, 'F', 0, 0, 0, 0, $1); }
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