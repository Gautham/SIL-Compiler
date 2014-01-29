struct Symbol *NewScope(char type) {
	struct Symbol *g = malloc(sizeof(struct Symbol));
	g->BP = g->Binding = SP;
	g->Size = 0;
	g->Type = type;
	g->parent = TopScope;
	g->next = 0;
	TopScope = g;
	return g;
}

void Install(char *Name, int Type, int Size) {
	struct Symbol *N = malloc(sizeof(struct Symbol));
	N->Name = Name;
	N->Type = Type;
	N->Size = Size;
	if (TopScope->Type == 'R') {
		N->Binding = TopScope->Size;
		TopScope->Size += Size;
		N->BP = TopScope->BP;
	}
	if (TopScope->Type == 'S') {
		N->Binding = SP;
		SP += Size;
	}
	N->next = 0;
	N->Scope = TopScope;
	N->parent = TopScope->parent;
	struct Symbol *P = TopScope;
	while (P->next) P = P->next;
	P->next = N;
}

struct Symbol * Lookup(char *, int);

void InstallVariable(struct Symbol *T, int size) {
	if (Lookup(T->Name, 0)) {
		printf("Variable \"%s\" was priorly declared within the present Scope.\n", T->Name);
		exit(0);
	}
	if (size > 0) Install(T->Name, DeclType, size);
	else {
		printf("Only +ve Integer Sizes are allowed for Array Dimensions.\n");
		exit(0);
	}
}

void *NewArgList() {
	Arg = malloc(sizeof(struct Symbol));
	Arg->Type = 'A';
	Arg->Size = 0;
	Arg->next = 0;
	Arg->parent = TopScope;
	TopScope = Arg;
}

void InstallArguement(struct Symbol *T, int size) {
	struct Symbol *TMP = Arg, *A = malloc(sizeof(struct Symbol));
	A->Name = T->Name;
	A->Type = DeclType;
	if (size > 0) A->Size = size;
	else {
		printf("Only +ve Integer Sizes are allowed for Array Dimensions.\n");
		exit(0);		
	}
	A->parent = TopScope->parent;
	A->Scope = Arg;
	A->Binding = TopScope->Size++;
	while (TMP->next != 0) TMP = TMP->next;
	TMP->next = A;
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

void InstallFunction(struct Symbol *T, struct Symbol *Scope) {
	struct Symbol *N = malloc(sizeof(struct Symbol));
	N->Name = T->Name;
	N->Type = T->Type;
	N->Scope = Scope;
	Scope->parent = Arg;
	N->ArgList = Arg;
	N->next = 0;
	if (!Functions) Functions = N;
	else {
		struct Symbol *P = Functions;
		while (P->next) P = P->next;
		P->next = N;
	}
}

struct Symbol *LookupFunction(char *Name) {
	struct Symbol *TMP = Functions;
	while (TMP && strcmp(TMP->Name, Name)) TMP = TMP->next;
	return TMP;
}

int TypeCheck(char a, char b, struct Node * p, struct Node * q, struct Symbol *m, struct Symbol *n) {
	int typeX, typeY;
	struct Symbol *TMP, *TMP2; 
	if (p) {
		switch (p->type) {
			case 'v':
				TMP = Lookup(p->g->Name, 1);
				if (TMP) typeX = (TMP->Type % 2);
				else typeX = -1;
				break;
			case 'F':
				TMP = LookupFunction(p->h->Name);
				if (TMP) typeX = (TMP->Type % 2);
				else typeX = -1;
			case 'a':
			case 'i':
				typeX = 1;
				break;
			case 'b':
			case 'l':
			case 'r':
				typeX = 0;
				break;
		}
		if (typeX == -1) {
			if (p->type == 'v') printf("Variable \"%s\" was not declared.\n", p->g->Name);
			else printf("Function \"%s()\" was not defined.\n", p->g->Name);
			exit(0);
		}
	} else if (m) typeX = m->Type % 2;
	else typeX = a % 2;
	if (q) {
		switch (q->type) {
			case 'v':
				TMP = Lookup(q->g->Name, 1);
				if (TMP) typeY = (TMP->Type % 2);
				else typeY = -1;
				break;
			case 'F':
				TMP = LookupFunction(q->h->Name);
				if (TMP) typeY = (TMP->Type % 2);
				else typeY = -1;
			case 'a':
			case 'i':
				typeY = 1;
				break;
			case 'b':
			case 'l':
			case 'r':
				typeY = 0;
				break;
		}
		if (typeY == -1) {
			if (q->type == 'v') printf("Variable \"%s\" was not declared.\n", q->g->Name);
			else printf("Function \"%s()\" was not defined.\n", q->g->Name);
			exit(0);
		}
	} else if (n) typeY = n->Type % 2;
	else typeY = b % 2;
	return (typeX == typeY);
}

