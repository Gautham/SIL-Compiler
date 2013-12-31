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

int value = 0;
int Prefix(struct Tree *p) {
	if (p->isOp) {
		printf("%c", p->val);
		switch (p->val) {
			case '+': return (Prefix(p->left) + Prefix(p->right));
			case '-': return (Prefix(p->left) - Prefix(p->right));
			case '*': return (Prefix(p->left) * Prefix(p->right));
			case '/': return (Prefix(p->left) / Prefix(p->right));
		}
	} else {
		printf("%d", p->val);
		return p->val;
	}
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
%type <node> exp;

%left '+' '-'
%left '*' '/'

%%


expression:	exp '\n'	{ printf(" = %d\n", Prefix($1)); exit(1); }

exp:	NUMBER	{
					struct Tree *p = malloc(sizeof(struct Tree));
					p->val = $1;
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