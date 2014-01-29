all: silc

lex.yy.c: silc.l
	flex silc.l

silc.tab.c silc.tab.h: silc.y
	bison -d silc.y

silc: definitions.h declarations.h variables.h parsetree.h parser.h silc.tab.c silc.tab.h lex.yy.c
	cc lex.yy.c silc.tab.c -o silc

clean:
	rm -rf lex.yy.c silc.tab.* silc sim.txt