struct gSymbol {
	char *Name;
	int Type, Size, Binding;
	struct gSymbol *next;
};

struct Node {
	int value, type;
	struct Node *t1, *t2, *t3;
	struct gSymbol *g;
};
