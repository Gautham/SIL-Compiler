%{
#include <stdio.h>
#include "tree.h"
#include <stdlib.h>

void yyerror(const char *str)
{
	fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
	return 1;
}

int Var[26];
int Evaluate(struct Tree *p) {
	if (p->isOp) {
		int t = Evaluate(p->left);
		switch (p->val) {
			case '+': return t + Evaluate(p->right);
			case '-': return t - Evaluate(p->right);
			case '*': return t * Evaluate(p->right);
			case '/': return t / Evaluate(p->right);
		}
	} else return p->val;
}

main()
{
	yyparse();
}
%}

%union {
	int number;
	struct Tree *node;
}

%token <number> NUMBER;
%token <number> ID;
%token <number> TOKREAD;
%token <number> TOKWRITE;
%token <number> BREAK;
%token <number> TOKEXIT;
%type <node> exp;
%type <number> expression;

%left '+' '-'
%left '*' '/'

%%

slist:	
		|
		slist statement BREAK;

statement:	
			|
			ID '=' expression { Var[$1] = $3;	}
			|
			TOKREAD '(' ID ')' { printf("? %c = ", $3); scanf("%d", &Var[$3]); }
			|
			TOKWRITE '(' expression ')' { printf("%d", $3); }
			|
			expression { printf("%d", $1); }
			|
			TOKEXIT { exit(0); }

expression:	exp	{ $$ = Evaluate($1); }

exp:	NUMBER	{
					struct Tree *p = malloc(sizeof(struct Tree));
					p->val = $1;
					p->isOp = 0;
					$$ = p;
				}
		|	ID	{
					struct Tree *p = malloc(sizeof(struct Tree));
					p->val = Var[$1];
					p->isOp = 0;
					$$ = p;
				}
		|	exp '+' exp	{
							struct Tree *p = malloc(sizeof(struct Tree));
							p->val = '+';
							p->isOp = 1;
							p->left = $1;
							p->right = $3;
							$$ = p;
						}
		|	exp '-' exp	{
							struct Tree *p = malloc(sizeof(struct Tree));
							p->val = '-';
							p->isOp = 1;
							p->left = $1;
							p->right = $3;
							$$ = p;
						}
		|	exp '*' exp	{
							struct Tree *p = malloc(sizeof(struct Tree));
							p->val = '*';
							p->isOp = 1;
							p->left = $1;
							p->right = $3;
							$$ = p;
						}
		|	exp '/' exp	{
							struct Tree *p = malloc(sizeof(struct Tree));
							p->val = '/';
							p->isOp = 1;
							p->left = $1;
							p->right = $3;
							$$ = p;
						}								
		|	'(' exp ')' { $$ = $2; }
%%