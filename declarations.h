int RCount = 0, BCount = 0, ArgCount = 0, LocalSP = 0, SP = 0, DeclType, FlagMain = 1;
struct Symbol *TopScope = 0, *Functions = 0, *Arg = 0, *FormalParam = 0;
struct Node *MainFunction = 0;
struct Parameters *Para = 0;
FILE * fp;