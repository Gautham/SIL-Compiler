#include <stdlib.h>
#include <string.h>


int VariableStore[200], RCount = 0, BCount = 0, MemCount = 0;
struct Symbol *TopScope = 0;
FILE * fp;

void Install(char *Name, int Type, int Size) {
	struct Symbol *N = malloc(sizeof(struct Symbol));
	N->Name = Name;
	N->Type = Type;
	N->Size = Size;
	N->Binding = MemCount;
	MemCount += Size;
	N->next = 0;
	N->parent = TopScope->parent;
	struct Symbol *P = TopScope;
	while (P->next) P = P->next;
	P->next = N;
}

struct Symbol *Lookup(char *Name, int isRecursive) {
	struct Symbol *L = TopScope, *TMP;
	do {
		TMP = L->next;
		while (TMP != 0 && strcmp(TMP->Name, Name) != 0) TMP = TMP->next;
		if (TMP != 0) return TMP;
		L = L->parent;
	} while (isRecursive && L);
	return 0;
}

void InstallVariable(struct Symbol *T, int type, int size, struct Node *sizeExp) {
	if (Lookup(T->Name, 0)) {
		printf("Variable \"%s\" was priorly declared within the present Scope.\n", T->Name);
		exit(0);
	}
	if (size) Install(T->Name, type, size);
	else {
		int t = Evaluate(sizeExp);
		if (t > 0) Install(T->Name, type, t);
		else printf("Only +ve Sizes are allowed for Array Dimensions.\n");
	}
}

struct Symbol *NewScope() {
	struct Symbol *g = malloc(sizeof(struct Symbol));
	g->parent = TopScope;
	g->next = 0;
	TopScope = g;
	return g;
}

struct Node *MakeNode(int value, int type, struct Node *t1, struct Node *t2, struct Node *t3, struct Symbol *g, struct Symbol *h) {
	switch (type) {
		case 'R': case 'A': case 'v':
			if (!Lookup(g->Name, 1)) {
				printf("Variable \"%s\" was not declared.\n", g->Name);
				exit(0);
			}
	}
	struct Node *t = malloc(sizeof(struct Node));
	t->value = value;
	t->type = type;
	t->t1 = t1;
	t->t2 = t2;
	t->t3 = t3;
	t->g = g;
	t->h = h;
	return t;
}

void Parse(struct Node *T) {
	int tmp1, tmp2, tmp3;
	struct Symbol *TMP;
	switch (T->type) {
		case 'i':
			fprintf(fp, "MOV R%d, %d\n", RCount++, T->value);
			break;
		case 'v':
			TMP = Lookup(T->g->Name, 1);			
			if (T->t1) {
				Parse(T->t1);
				fprintf(fp, "MOV R%d, %d\n", RCount, TMP->Binding);
				fprintf(fp, "ADD R%d, R%d\n", RCount - 1, RCount);
				fprintf(fp, "MOV R%d, [R%d]\n", RCount - 1, RCount - 1);
			} else fprintf(fp, "MOV R%d, [%d]\n", RCount++, TMP->Binding);
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
			TMP = Lookup(T->g->Name, 1);
			fprintf(fp, "IN R%d\n", RCount++);
			if (T->t1) {
				Parse(T->t1);
				fprintf(fp, "MOV R%d, %d\n", RCount, TMP->Binding);
				fprintf(fp, "ADD R%d, R%d\n", RCount - 1, RCount);
				fprintf(fp, "MOV [R%d], R%d\n", RCount - 1, RCount - 2);
				RCount -= 2;
			} else fprintf(fp, "MOV [%d], R%d\n", TMP->Binding, --RCount);
			break;
		case 'W':
			Parse(T->t1);
			fprintf(fp, "OUT R%d\n", --RCount);
			break;
		case 'A':
			Parse(T->t2);
			TMP = Lookup(T->g->Name, 1);
			if (T->t1) {
				Parse(T->t1);
				fprintf(fp, "MOV R%d, %d\n", RCount, TMP->Binding);
				fprintf(fp, "ADD R%d, R%d\n", RCount - 1, RCount);				
				fprintf(fp, "MOV [R%d], R%d\n", RCount - 1, RCount - 2);
				RCount -= 2;
			} else fprintf(fp, "MOV [%d], R%d\n", TMP->Binding, --RCount);
			break;		
		case 'S':
			if (T->t1) Parse(T->t1);
			if (T->t2) Parse(T->t2);
			break;
		case 'C':
			Parse(T->t1);
			tmp1 = BCount++;
			if (T->t3) fprintf(fp, "JZ R%d, ELSEBL%d\n", --RCount, tmp1);
			else fprintf(fp, "JZ R%d, ENDBL%d\n", --RCount, tmp1);
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
			tmp1 = BCount++;
			fprintf(fp, "BL%d:\n", tmp1);
			Parse(T->t1);
			TopScope = T->g;
			fprintf(fp, "JZ R%d, ENDBL%d\n", --RCount, tmp1);
			Parse(T->t2);
			fprintf(fp, "JMP BL%d\n", tmp1);
			TopScope = TopScope->parent;			
			fprintf(fp, "ENDBL%d:\n", tmp1);
			break;
	}
}

int Evaluate(struct Node *T) {
	int tmp1, tmp2, tmp3;
	struct Symbol *TMP;
	switch (T->type) {
		case 'i': return T->value;
		case 'v':
			TMP = Lookup(T->g->Name, 1);
			tmp1 = 0;
			if (T->t1) tmp1 = Evaluate(T->t1);
			if (tmp1 >= TMP->Size) {
				printf("Array Index Out Of Bounds\n");
				exit(0);
			}
			return VariableStore[tmp1 + TMP->Binding];
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
		case 'R':
			TMP = Lookup(T->g->Name, 1);
			printf("? %s = ", TMP->Name);
			tmp1 = 0;
			if (T->t1) tmp1 = Evaluate(T->t1);
			if (tmp1 >= TMP->Size) {
				printf("Array Index Out Of Bounds\n");
				exit(0);
			}
			scanf("%d", &VariableStore[tmp1 + TMP->Binding]);
			return 1;
		case 'W':
			printf("%d\n", Evaluate(T->t1));
			return 1;
		case 'A':
			TMP = Lookup(T->g->Name, 1);
			tmp1 = 0;
			if (T->t1) tmp1 = Evaluate(T->t1);			
			if (tmp1 >= TMP->Size) {
				printf("Array Index Out Of Bounds\n");
				exit(0);
			}
			VariableStore[tmp1 + TMP->Binding] = Evaluate(T->t2);
			return 1;
		case 'S':
			if (T->t1) Evaluate(T->t1);
			if (T->t2) Evaluate(T->t2);
			return 1;
		case 'C':
			if (Evaluate(T->t1)) {
				TopScope = T->g;
				Evaluate(T->t2);
				TopScope = TopScope->parent;
			} else if (T->t3) {
				TopScope = T->h;
				Evaluate(T->t3);
				TopScope = TopScope->parent;
			} 
			return 1;
		case 'L':
			while (Evaluate(T->t1)) {
				TopScope = T->g;
				Evaluate(T->t2);
				TopScope = TopScope->parent;
			}
			return 1;
	}
}