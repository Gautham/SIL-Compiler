int LineCount = 0, RegisterCount = 0, BlockCount = 0, ArgCount = 0, SP = 0, DeclType;
struct Symbol *TopScope = 0, *Functions = 0, *Arg = 0, *FormalParam = 0;
struct Node *MainFunction = 0;
FILE * fp;

void Generate(struct Node *T);