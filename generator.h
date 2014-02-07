#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "declarations.h"
#include "variables.h"
#include "parsetree.h"

void GetMemoryLocation(struct Symbol *G, struct Node *Offset) {
	switch  (G->Scope->Type) {
		case 'S':
			fprintf(fp, "MOV R%d, %d\n", RegisterCount, G->Binding);
			break;
		case 'R':
			fprintf(fp, "MOV R%d, BP\n", RegisterCount);
			fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, G->Binding + 1);
			fprintf(fp, "ADD R%d, R%d\n", RegisterCount, RegisterCount + 1);
			break;
		case 'A':
			fprintf(fp, "MOV R%d, BP\n", RegisterCount);
			fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, Arg->Size - G->Binding + 2);
			fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
			if (G->BP) fprintf(fp, "MOV R%d, [R%d]\n", RegisterCount, RegisterCount);
			break;
	}
	++RegisterCount;
	if (Offset) {
		Generate(Offset);
		fprintf(fp, "ADD R%d, R%d\n", RegisterCount - 2, RegisterCount - 1);
		RegisterCount -= 1;
	}
}


void Generate(struct Node *T) {
	int tmp1, tmp2, tmp3;
	struct Symbol *TMP, *TMP2;
	struct Node *param;
	switch (T->type) {
		case 'b':
		case 'i':
			fprintf(fp, "MOV R%d, %d\n", RegisterCount++, T->value);
			break;
		case 'v':
			TMP = Lookup(T->g->Name, 1);
			GetMemoryLocation(TMP, T->t1);
			fprintf(fp, "MOV R%d, [R%d]\n", RegisterCount - 1, RegisterCount - 1);
			break;
		case 'a':
			Generate(T->t1);
			tmp2 = RegisterCount - 1;
			Generate(T->t2);
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
			Generate(T->t1);
			tmp2 = RegisterCount - 1;
			Generate(T->t2);
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
			Generate(T->t1);
			tmp2 = RegisterCount - 1;
			switch(T->value) {
				case 'a':
					Generate(T->t2);
					fprintf(fp, "MUL R%d, R%d\n", tmp2, --RegisterCount);
					break;
				case 'o':
					Generate(T->t2);
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
			GetMemoryLocation(TMP, T->t1);
			fprintf(fp, "MOV [R%d], R%d\n", RegisterCount - 1, RegisterCount - 2);
			RegisterCount -= 2;
			break;
		case 'W':
			Generate(T->t1);
			fprintf(fp, "OUT R%d\n", --RegisterCount);
			break;
		case 'A':
			Generate(T->t2);
			TMP = Lookup(T->g->Name, 1);
			GetMemoryLocation(TMP, T->t1);
			fprintf(fp, "MOV [R%d], R%d\n", RegisterCount - 1, RegisterCount - 2);
			RegisterCount -= 2;
			break;
		case 'S':
			if (T->t1) Generate(T->t1);
			if (T->t2) Generate(T->t2);
			break;
		case 'C':
			Generate(T->t1);
			tmp1 = BlockCount++;
			if (T->t3) fprintf(fp, "JZ R%d, ELSEBL%d\n", --RegisterCount, tmp1);
			else fprintf(fp, "JZ R%d, ENDBL%d\n", --RegisterCount, tmp1);
			TopScope = T->g;
			Generate(T->t2);
			fprintf(fp, "JMP ENDBL%d\n", tmp1);
			if (T->t3) {
				TopScope = T->h;
				fprintf(fp, "ELSEBL%d:\n", tmp1);
				Generate(T->t3);
			}
			TopScope = TopScope->parent;
			fprintf(fp, "ENDBL%d:\n", tmp1);
			break;
		case 'L':
			tmp1 = BlockCount++;
			fprintf(fp, "BL%d:\n", tmp1);
			Generate(T->t1);
			TopScope = T->g;
			fprintf(fp, "JZ R%d, ENDBL%d\n", --RegisterCount, tmp1);
			Generate(T->t2);
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
				TMP2 = TMP->ArgList->next;
				while (param != 0) {
					if (TMP2->BP) GetMemoryLocation(Lookup(param->t1->g->Name, 1), param->t1->t1);
					else Generate(param->t1);
					fprintf(fp, "PUSH R%d\n", --RegisterCount);
					param = param->t3;
					TMP2 = TMP2->next;
				}
				fprintf(fp, "PUSH R%d\n", RegisterCount);
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

void GenerateFunction(struct Node * T, struct Symbol *ID, struct Symbol *FunctScope, struct Node * RValue) {
	FunctScope->parent = TopScope;
	Arg = TopScope;
	TopScope = FunctScope;
	if (!TypeCheck(0, 0, RValue, 0, 0, ID)) {
		if (ID->Type == 1) printf("Returning Boolean Value from Integer Function \"%s()\" is not permitted.\n", ID->Name);
		else printf("Returning Integer Value from Boolean Function \"%s()\" is not permitted.\n", ID->Name);
		exit(0);
	}
	fprintf(fp, "%s:\n", ID->Name);
	fprintf(fp, "PUSH BP\n");
	fprintf(fp, "MOV BP, SP\n");
	fprintf(fp, "MOV R%d, SP\n", RegisterCount);
	fprintf(fp, "MOV R%d, %d\n", RegisterCount + 1, FunctScope->Size);
	fprintf(fp, "ADD R%d, R%d\n", RegisterCount, RegisterCount + 1);
	fprintf(fp, "MOV SP, R%d\n", RegisterCount);
	Generate(T);
	Generate(RValue);
	fprintf(fp, "MOV SP, BP\n");
	fprintf(fp, "POP BP\n");
	fprintf(fp, "MOV R%d, SP\n", RegisterCount);
	fprintf(fp, "MOV R%d, 1\n", RegisterCount + 1);
	fprintf(fp, "SUB R%d, R%d\n", RegisterCount, RegisterCount + 1);
	fprintf(fp, "MOV [R%d], R%d\n", RegisterCount, RegisterCount - 1);
	--RegisterCount;
	fprintf(fp, "RET\n");
	TopScope = TopScope->parent->parent;
}