#include <stdlib.h>
#include <string.h>


int VariableStore[200], RCount = 0, BCount = 0, SP = 0, BP = 0;
struct gSymbol * Global = 0;
FILE * fp;

struct Node *MakeNode(int value, int type, struct Node *t1, struct Node *t2, struct Node *t3, struct gSymbol *g) {
	struct Node *t = malloc(sizeof(struct Node));
	t->value = value;
	t->type = type;
	t->t1 = t1;
	t->t2 = t2;
	t->t3 = t3;
	t->g = g;
	return t;
}

void Install(char *Name, int Type, int Size) {
	struct gSymbol *N = malloc(sizeof(struct gSymbol));
	N->Name = Name;
	N->Type = Type;
	N->Size = Size;
	N->Binding = SP;
	SP += Size;
	N->next = 0;
	if (Global) {
		struct gSymbol *TMP = Global;
		while (TMP->next) TMP = TMP->next;
		TMP->next = N;
	} else Global = N;
}

struct gSymbol *gLookup(char *Name) {
	struct gSymbol *TMP = Global;
	while (TMP != 0 && strcmp(TMP->Name, Name) != 0) TMP = TMP->next;
	return TMP;
}

/*
void Parse(struct Node *T) {
	int tmp1, tmp2, tmp3;
	switch (T->type) {
		case 'i':
			fprintf(fp, "MOV R%d, %d\n", RCount++, T->value);
			break;
		case 'v':
			fprintf(fp, "MOV R%d, %d\n", RCount, T->value - 'a');
			fprintf(fp, "MOV R%d, BP\n", RCount + 1);
			fprintf(fp, "ADD R%d, R%d\n", RCount, RCount + 1);
			fprintf(fp, "MOV R%d, [R%d]\n", RCount, RCount);
			++RCount;
			break;		
		case 'a':
			Parse(T->t1);
			tmp2 = RCount - 1;
			Parse(T->t2);
			switch (T->value) {
				case '+':
					fprintf(fp, "ADD R%d, R%d\n", tmp2, --RCount);
					break;
				case '-':
					fprintf(fp, "SUB R%d, R%d\n", tmp2, --RCount);
					break;
				case '*':
					fprintf(fp, "MUL R%d, R%d\n", tmp2, --RCount);
					break;
				case '/':
					fprintf(fp, "DIV R%d, R%d\n", tmp2, --RCount);
					break;
				case '%':
					fprintf(fp, "MOD R%d, R%d\n", tmp2, --RCount);
					break;
			}
			break;
		case 'r':
			Parse(T->t1);
			tmp2 = RCount - 1;
			Parse(T->t2);
			switch (T->value) {
				case '>':
					fprintf(fp, "GT R%d, R%d\n", tmp2, --RCount);
					break;
				case '<':
					fprintf(fp, "LT R%d, R%d\n", tmp2, --RCount);
					break;
				case '=':
					fprintf(fp, "EQ R%d, R%d\n", tmp2, --RCount);
					break;
				case 'g':
					fprintf(fp, "GE R%d, R%d\n", tmp2, --RCount);
					break;
				case 'l':
					fprintf(fp, "LE R%d, R%d\n", tmp2, --RCount);
					break;
			}
			break;
		case 'R':
			fprintf(fp, "IN R%d\n", RCount);
			fprintf(fp, "MOV R%d, %d\n", RCount + 1, T->value - 'a');
			fprintf(fp, "MOV R%d, BP\n", RCount + 2);			
			fprintf(fp, "ADD R%d, R%d\n", RCount + 1, RCount + 2);
			fprintf(fp, "MOV [R%d], R%d\n", RCount + 1, RCount);
			break;
		case 'W':
			Parse(T->t1);
			fprintf(fp, "OUT R%d\n", --RCount);
			break;
		case 'A':
			Parse(T->t1);
			fprintf(fp, "MOV R%d, %d\n", RCount, T->value - 'a');
			fprintf(fp, "MOV R%d, BP\n", RCount + 1);			
			fprintf(fp, "ADD R%d, R%d\n", RCount, RCount + 1);
			fprintf(fp, "MOV [R%d], R%d\n", RCount, RCount - 1);
			RCount -= 1;
			break;		
		case 'S':
			if (T->t2) Parse(T->t2);
			if (T->t1) Parse(T->t1);
			break;
		case 'C':
			Parse(T->t1);
			tmp1 = BCount++;
			if (T->t3) fprintf(fp, "JZ R%d, ELSEBL%d\n", --RCount, tmp1);
			else fprintf(fp, "JZ R%d, BL%d\n", --RCount, tmp1);
			Parse(T->t2);
			fprintf(fp, "JMP BL%d\n", tmp1);
			if (T->t3) {
				fprintf(fp, "ELSEBL%d:\n", tmp1);
				Parse(T->t3);
			}
			fprintf(fp, "BL%d:\n", tmp1);
			break;
		case 'L':
			tmp1 = BCount++;
			fprintf(fp, "BL%d:\n", tmp1);
			Parse(T->t1);
			fprintf(fp, "JZ R%d, ENDBL%d\n", --RCount, tmp1);
			Parse(T->t2);
			fprintf(fp, "JMP BL%d\n", tmp1);
			fprintf(fp, "ENDBL%d:\n", tmp1);
			break;
	}
}
*/

int Evaluate(struct Node *T) {
	int tmp1, tmp2, tmp3;
	struct gSymbol *TMP;
	switch (T->type) {
		case 'i': return T->value;
		case 'v':
			TMP = gLookup(T->g->Name);
			if (!TMP) { printf("Variable Not Declared!"); exit(0); }
			return VariableStore[TMP->Binding];

		case 'a':
			tmp1 = Evaluate(T->t1);
			tmp3 = Evaluate(T->t2);
			switch (T->value) {
				case '+': return tmp1 + tmp3;
				case '-': return tmp1 - tmp3;
				case '*': return tmp1 * tmp3;
				case '/': return tmp1 / tmp3;
				case '%': return tmp1 % tmp3;
			}
		case 'r':
			tmp1 = Evaluate(T->t1);
			tmp3 = Evaluate(T->t2);
			switch (T->value) {
				case '>': return (tmp1 > tmp3);
				case '<': return (tmp1 < tmp3);
				case '=': return (tmp1 == tmp3);
				case 'g': return (tmp1 >= tmp3);
				case 'l': return (tmp1 <= tmp3);
			}
		case 'D':
			Install(T->g->Name, 1, 1);
			break;			
		case 'R':
			TMP = gLookup(T->g->Name);
			if (!TMP) { printf("Variable Not Declared!"); exit(0); }
			printf("? %s = ", TMP->Name);
			scanf("%d", &VariableStore[TMP->Binding]);
			return 1;
		case 'W':
			printf("%d\n", Evaluate(T->t1));
			return 1;
		case 'A':
			TMP = gLookup(T->g->Name);
			if (!TMP) { printf("Variable Not Declared!"); exit(0); }
			VariableStore[TMP->Binding] = Evaluate(T->t1);
			return 1;
		case 'S':
			if (T->t2) Evaluate(T->t2);
			if (T->t1) Evaluate(T->t1);
			return 1;
		case 'C':
			if (Evaluate(T->t1)) Evaluate(T->t2); else if (T->t3) Evaluate(T->t3);
			return 1;
		case 'L':
			while (Evaluate(T->t1)) Evaluate(T->t2);
			return 1;
	}
}