#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "declarations.h"
#include "variables.h"
#include "parsetree.h"

void Parse(struct Node *T) {
	int tmp1, tmp2, tmp3;
	struct Symbol *TMP;
	struct Node *param;
	switch (T->type) {
		case 'b':
		case 'i':
			fprintf(fp, "MOV R%d, %d\n", RegisterCount++, T->value);
			break;
		case 'v':
			TMP = Lookup(T->g->Name, 1);			
			if (T->t1) Parse(T->t1);
			switch  (TMP->Scope->Type) {
				case 'S':	
					fprintf(fp, "MOV R%d, %d\n", RegisterCount, TMP->Binding);
					break;
				case 'R':
					fprintf(fp, "MOV R%d, SP\n", RegisterCount);
					fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, TMP->Scope->Size - TMP->Binding - 1);
					fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
					break;
				case 'A':
					fprintf(fp, "MOV R%d, BP\n", RegisterCount);
					fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, Arg->Size - TMP->Binding  + 1);
					fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
			}
			if (T->t1) {
				fprintf(fp, "ADD R%d, R%d\n", RegisterCount - 1, RegisterCount);
				fprintf(fp, "MOV R%d, [R%d]\n", RegisterCount - 1, RegisterCount - 1);
			}
			else {
				fprintf(fp, "MOV R%d, [R%d]\n", RegisterCount, RegisterCount);
				++RegisterCount;
			}
			break;		
		case 'a':
			Parse(T->t1);
			tmp2 = RegisterCount - 1;
			Parse(T->t2);
			switch (T->value) {
				case '+':
					fprintf(fp, "ADD R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case '-':
					fprintf(fp, "SUB R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case '*':
					fprintf(fp, "MUL R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case '/':
					fprintf(fp, "DIV R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case '%':
					fprintf(fp, "MOD R%d, R%d\n", tmp2, --RegisterCount);
					break;
			}
			break;
		case 'r':
			Parse(T->t1);
			tmp2 = RegisterCount - 1;
			Parse(T->t2);
			switch (T->value) {
				case '>':
					fprintf(fp, "GT R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case '<':
					fprintf(fp, "LT R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case '=':
					fprintf(fp, "EQ R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case 'g':
					fprintf(fp, "GE R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case 'l':
					fprintf(fp, "LE R%d, R%d\n", tmp2, --RegisterCount);
					break;
			}
			break;
		case 'l':
			Parse(T->t1);
			tmp2 = RegisterCount - 1;
			switch(T->value) {
				case 'a':
					Parse(T->t2);
					fprintf(fp, "MUL R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case 'o':
					Parse(T->t2);
					fprintf(fp, "ADD R%d, R%d\n", tmp2, --RegisterCount);					
					fprintf(fp, "MOV R%d, 0\n", RegisterCount);
					fprintf(fp, "NE R%d, R%d\n", tmp2, RegisterCount);
					break;
				case 'n':
					fprintf(fp, "MOV R%d, 0\n", RegisterCount);
					fprintf(fp, "EQ R%d, R%d\n", tmp2, RegisterCount);	
					break;
			}
			break;
		case 'R':
			TMP = Lookup(T->g->Name, 1);
			fprintf(fp, "IN R%d\n", RegisterCount++);
			if (T->t1) Parse(T->t1);
			switch  (TMP->Scope->Type) {
				case 'S':	
					fprintf(fp, "MOV R%d, %d\n", RegisterCount, TMP->Binding);
					break;
				case 'R':
					fprintf(fp, "MOV R%d, SP\n", RegisterCount);
					fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, TMP->Scope->Size - TMP->Binding - 1);
					fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
					break;
				case 'A':
					fprintf(fp, "MOV R%d, BP\n", RegisterCount);
					fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, Arg->Size - TMP->Binding  + 1);
					fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
			}
			if (T->t1) {
				fprintf(fp, "ADD R%d, R%d\n", RegisterCount - 1, RegisterCount);
				fprintf(fp, "MOV [R%d], R%d\n", RegisterCount - 1, RegisterCount - 2);
				RegisterCount -= 2;
			}
			else {
				fprintf(fp, "MOV [R%d], R%d\n", RegisterCount, RegisterCount - 1);
				RegisterCount -= 1;
			}
			break;
		case 'W':
			Parse(T->t1);
			fprintf(fp, "OUT R%d\n", --RegisterCount);
			break;
		case 'A':
			Parse(T->t2);
			TMP = Lookup(T->g->Name, 1);
			if (T->t1) Parse(T->t1);
			switch  (TMP->Scope->Type) {
				case 'S':	
					fprintf(fp, "MOV R%d, %d\n", RegisterCount, TMP->Binding);
					break;
				case 'R':
					fprintf(fp, "MOV R%d, SP\n", RegisterCount);
					fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, TMP->Scope->Size - TMP->Binding - 1);
					fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
					break;
				case 'A':
					fprintf(fp, "MOV R%d, BP\n", RegisterCount);
					fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, Arg->Size - TMP->Binding  + 1);
					fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
			}
			if (T->t1) {
				fprintf(fp, "ADD R%d, R%d\n", RegisterCount - 1, RegisterCount);
				fprintf(fp, "MOV [R%d], R%d\n", RegisterCount - 1, RegisterCount - 2);
				RegisterCount -= 2;
			}
			else {
				fprintf(fp, "MOV [R%d], R%d\n", RegisterCount, RegisterCount - 1);
				RegisterCount -= 1;
			}
			break;		
		case 'S':
			if (T->t1) Parse(T->t1);
			if (T->t2) Parse(T->t2);
			break;
		case 'C':
			Parse(T->t1);
			tmp1 = BlockCount++;
			if (T->t3) fprintf(fp, "JZ R%d, ELSEBL%d\n", --RegisterCount, tmp1);
			else fprintf(fp, "JZ R%d, ENDBL%d\n", --RegisterCount, tmp1);
			TopScope = T->g;
			Parse(T->t2);
			fprintf(fp, "JMP ENDBL%d\n", tmp1);
			if (T->t3) {
				TopScope = T->h;
				fprintf(fp, "ELSEBL%d:\n", tmp1);
				Parse(T->t3);
			}
			TopScope = TopScope->parent;
			fprintf(fp, "ENDBL%d:\n", tmp1);
			break;
		case 'L':
			tmp1 = BlockCount++;
			fprintf(fp, "BL%d:\n", tmp1);
			Parse(T->t1);
			TopScope = T->g;
			fprintf(fp, "JZ R%d, ENDBL%d\n", --RegisterCount, tmp1);
			Parse(T->t2);
			fprintf(fp, "JMP BL%d\n", tmp1);
			TopScope = TopScope->parent;			
			fprintf(fp, "ENDBL%d:\n", tmp1);
			break;
		case 'F':
			TMP = LookupFunction(T->h->Name);
			if (TMP) {
				tmp1 = 0;
				while (tmp1 < RegisterCount) fprintf(fp, "PUSH R%d\n", tmp1++);
				param = T->t1;
				while (param != 0) {
					Parse(param->t1);
					fprintf(fp, "PUSH R%d\n", --RegisterCount);
					param = param->t3;
				}
				fprintf(fp, "PUSH R%d\n", RegisterCount + 1);
				fprintf(fp, "CALL %s\n", TMP->Name);
				fprintf(fp, "POP R%d\n", RegisterCount++);
				fprintf(fp, "MOV R%d, SP\n", RegisterCount);
				fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, TMP->ArgList->Size);
				fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
				fprintf(fp, "MOV SP, R%d\n", RegisterCount);
				tmp1 = RegisterCount - 2;
				while (tmp1 >= 0) fprintf(fp, "POP R%d\n", tmp1--);				
			} else {
				printf("Function %s Not Defined.\n", T->h->Name);
				exit(0);
			}
			break;
	}
}

void ParseFunction(struct Node * T, struct Symbol *ID, struct Symbol *FunctScope, struct Node * RValue) {
	FunctScope->parent = TopScope;
	Arg = TopScope;
	TopScope = FunctScope;
	if (!TypeCheck(0, 0, RValue, 0, 0, ID)) {
		if (ID->Type == 1) printf("Returning Boolean Value from Integer Function \"%s()\" is not permitted.\n", ID->Name);						
		else printf("Returning Integer Value from Boolean Function \"%s()\" is not permitted.\n", ID->Name);
		exit(0);		
	}	
	fprintf(fp, "%s:\n", ID->Name);
	fprintf(fp, "MOV R%d, BP\n", RegisterCount++);
	fprintf(fp, "MOV BP, SP\n");
	fprintf(fp, "MOV R%d, SP\n", RegisterCount);
	fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, FunctScope->Size);	
	fprintf(fp, "ADD R%d, R%d\n", RegisterCount, RegisterCount + 1);
	fprintf(fp, "MOV SP, R%d\n", RegisterCount);			
	Parse(T);
	Parse(RValue);
	fprintf(fp, "MOV R%d, SP\n", RegisterCount);
	fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, 1 + FunctScope->Size);
	fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
	fprintf(fp, "MOV [R%d], R%d\n", RegisterCount, RegisterCount - 1);
	RegisterCount -= 2;
	fprintf(fp, "MOV BP, R%d\n", RegisterCount);
	fprintf(fp, "MOV R%d, SP\n", RegisterCount);
	fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, FunctScope->Size);	
	fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
	fprintf(fp, "MOV SP, R%d\n", RegisterCount);
	fprintf(fp, "RET\n");
	TopScope = TopScope->parent->parent;
}