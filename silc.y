%{
#include <stdio.h>
#include "tree.h"
#include "treeHelper.h"
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
			case '%': return t % Evaluate(p->right);
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

%token <number> NUMBER ID TOKREAD TOKWRITE BREAK TOKEXIT TOKEQUAL;

%type <node> exp;
%type <number> expression;


%left TOKEQUAL
%left '<' '>'
%left '+' '-'
%left '*' '/' '%'

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
			TOKEXIT { exit(0); }

expression:	exp	{ $$ = Evaluate($1); }

exp:	NUMBER	{ $$ = makeNumTree($1); }
		|	ID	{ $$ = makeNumTree(Var[$1]); }
		|	exp '+' exp	{ $$ = makeOpTree('+', $1, $3); }
		|	exp '-' exp	{ $$ = makeOpTree('-', $1, $3); }
		|	exp '*' exp	{ $$ = makeOpTree('*', $1, $3); }
		|	exp '/' exp	{ $$ = makeOpTree('/', $1, $3); }
		|	exp '%' exp	{ $$ = makeOpTree('%', $1, $3); }
		|	exp TOKEQUAL exp { $$ = (Evaluate($1) == Evaluate($3)) ? makeNumTree(1) : makeNumTree(0); }
		|	'(' exp ')' { $$ = $2; }
%%