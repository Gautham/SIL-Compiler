#include <stdlib.h>
#include <string.h>


int Variable[26], RCount = 0;
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

int Evaluate(struct Node *T) {
	int tmp1, tmp2, tmp3;
	switch (T->type) {
		case 'i':
			fprintf(fp, "MOV R%d, %d\n", RCount++, T->value);
			return T->value;
		case 'v': return Variable[T->value - 'a'];
		case 'a':
			tmp1 = Evaluate(T->t1);
			tmp2 = RCount - 1;
			tmp3 = Evaluate(T->t2);
			switch (T->value) {
				case '+':
					fprintf(fp, "ADD R%d, R%d\n", tmp2, --RCount);
					return tmp1 + tmp3;
				case '-':
					fprintf(fp, "SUB R%d, R%d\n", tmp2, --RCount);
					return tmp1 - tmp3;
				case '*':
					fprintf(fp, "MUL R%d, R%d\n", tmp2, --RCount);
					return tmp1 * tmp3;
				case '/':
					fprintf(fp, "DIV R%d, R%d\n", tmp2, --RCount);
					return tmp1 / tmp3;
				case '%':
					fprintf(fp, "MOD R%d, R%d\n", tmp2, --RCount);
					return tmp1 % tmp3;
			}
		case '>': return Evaluate(T->t1) > Evaluate(T->t2);
		case '<': return Evaluate(T->t1) < Evaluate(T->t2);
		case '=': return Evaluate(T->t1) == Evaluate(T->t2);
		case 'g': return Evaluate(T->t1) >= Evaluate(T->t2);
		case 'l': return Evaluate(T->t1) <= Evaluate(T->t2);
		case 'R':
			printf("? %c = ", T->value);
			scanf("%d", &Variable[T->value - 'a']);
			return 1;
		case 'W':
			printf("%d\n", Evaluate(T->t1));
			fprintf(fp, "OUT R%d\n", --RCount);
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
				
	}
}