#include <stdlib.h>

struct Tree *makeOpTree(int val, struct Tree *left, struct Tree *right) {
	struct Tree *t = malloc(sizeof(struct Tree));
	t->val = val;
	t->isOp = 1;
	t->left = left;
	t->right = right;
	return t;
}

struct Tree *makeNumTree(int val) {
	struct Tree *t = malloc(sizeof(struct Tree));
	t->val = val;
	t->isOp = 0;
	t->left = 0;
	t->right = 0;
	return t;
}