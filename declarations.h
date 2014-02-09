int LineCount = 1, RegisterCount = 0, BlockCount = 0, SP = 0, DeclType;
struct Symbol *GlobalScope = 0, *TopScope = 0, *Functions = 0, *Arg = 0, *FormalParam = 0;
struct Node *MainFunction = 0;
FILE * fp;

void Generate(struct Node *T);