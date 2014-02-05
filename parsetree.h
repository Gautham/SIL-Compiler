struct Node *MakeNode(int value, int type, struct Node *t1, struct Node *t2, struct Node *t3, struct Symbol *g, struct Symbol *h) {
	struct Symbol *F, *TMP;
	struct Node *param;
	int tmp;
	switch (type) {
		case 'A':
			F = Lookup(g->Name, 1);
			if (!F) {
				printf("Variable \"%s\" was not declared.\n", g->Name);
				exit(0);
			}
			if (t2->type == 'F') {
				TMP = LookupFunction(t2->h->Name);
				if (!TMP) {
					printf("Function \"%s()\" was not declared.\n", t2->h->Name);
					exit(0);
				} else {
					if (!TypeCheck(0, 0, 0, 0, TMP, F)) {
						if (F->Type == 1) printf("Assignment Of Boolean Value to Integer Variable \"%s\" is not permitted.\n", F->Name);						
						else printf("Assignment Of Integer Value to Boolean Variable \"%s\" is not permitted.\n", F->Name);
						exit(0);
					}
				}
			} else {
				if (!TypeCheck(0, 0, t2, 0, 0, F)) {
					if (F->Type == 1) printf("Assignment Of Boolean Value to Integer Variable \"%s\" is not permitted.\n", F->Name);						
					else printf("Assignment Of Integer Value to Boolean Variable \"%s\" is not permitted.\n", F->Name);
					exit(0);
				}
			}
			break;
		case 'R': case 'v':
			if (!Lookup(g->Name, 1)) {
				printf("Variable \"%s\" was not declared.\n", g->Name);
				exit(0);
			}
			break;
		case 'r':
		case 'a':
			if (TypeCheck(2, 0, 0, t1, 0, 0) || TypeCheck(2, 0, 0, t2, 0, 0)) {
				printf("Usage Of Boolean Operands for Expression is prohibited.\n");
				exit(0);	
			}
			break;
		case 'l':
			if (TypeCheck(1, 0, 0, t1, 0, 0) || ((value != 'n') && TypeCheck(1, 0, 0, t2, 0, 0))) {
				printf("Usage Of Arithmetic Expression for Logical Operation is prohibited.\n");
				exit(0);
			}
			break;
		case 'F':
			F = LookupFunction(h->Name);
			if (!F) {
				printf("Function \"%s()\" was not declared.\n", h->Name);
				exit(0);
			}
			tmp = 0;
			TMP = F->ArgList->next;
			param = t1;
			while (param && TMP) {
				if (!TypeCheck(0, 0, param->t1, 0, 0, TMP)) {
					printf("Arguement Mismatch in Call to Function \"%s()\"\n", h->Name);
					exit(0);
				}
				TMP = TMP->next;
				param = param->t3;
			}
			if (param || TMP) {
				printf("Arguement Mismatch in Call to Function \"%s()\"\n", h->Name);
				exit(0);				
			}
			break;
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