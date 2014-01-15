struct Symbol {
	char *Name;
	int Type, Size, Binding;
	struct Symbol *next, *parent;
};

struct Node {
	int value, type;
	struct Node *t1, *t2, *t3;
	struct Symbol *g, *h;
};
