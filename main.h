#include <stdlib.h>

int Variable[26];

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
	switch (T->type) {
		case 'i': return T->value;
		case 'v': return Variable[T->value - 'a'];
		case '+': return Evaluate(T->t1) + Evaluate(T->t2);
		case '-': return Evaluate(T->t1) - Evaluate(T->t2);
		case '*': return Evaluate(T->t1) * Evaluate(T->t2);
		case '/': return Evaluate(T->t1) / Evaluate(T->t2);
		case '%': return Evaluate(T->t1) % Evaluate(T->t2);
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