%{
#include <stdio.h>
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

%token NUMBER;

%left '+' '-'
%left '*' '/'

%%


expression:	exp '\n'	{ printf(" = %d", $1); exit(1); }

exp:	NUMBER { $$ = $1; printf("%d", $1); }
		|	exp '+' exp { $$ = $1 + $3; printf("+"); }
		|	exp '-' exp { $$ = $1 - $3; printf("-"); }
		|	exp '*' exp { $$ = $1 * $3; printf("*"); }
		|	exp '/' exp { $$ = $1 / $3; printf("/"); }
		|	'(' exp ')' { $$ = $2; }
%%