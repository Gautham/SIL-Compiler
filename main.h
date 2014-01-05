#include <stdlib.h>
#include <string.h>


int Variable[26], RCount = 0, BCount = 0;
FILE * fp;

struct Node *MakeNode(int value, int type, struct Node *t1, struct Node *t2, struct Node *t3) {
	struct Node *t = malloc(sizeof(struct Node));
	t->value = value;
	t->type = type;
	t->t1 = t1;
	t->t2 = t2;
	t->t3 = t3;
	return t;
}

void Parse(struct Node *T) {
	int tmp1, tmp2, tmp3;
	switch (T->type) {
		case 'i':
			fprintf(fp, "MOV R%d, %d\n", RCount++, T->value);
			break;
		case 'v': break;
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
			break;
		case 'W':
			Parse(T->t1);
			fprintf(fp, "OUT R%d\n", --RCount);
			break;
		case 'A':
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

int Evaluate(struct Node *T) {
	int tmp1, tmp2, tmp3;
	switch (T->type) {
		case 'i': return T->value;
		case 'v': return Variable[T->value - 'a'];
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
			printf("? %c = ", T->value);
			scanf("%d", &Variable[T->value - 'a']);
			return 1;
		case 'W':
			printf("%d\n", Evaluate(T->t1));
			return 1;
		case 'A':
			Variable[T->value - 'a'] = Evaluate(T->t1);
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