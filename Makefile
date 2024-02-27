CC	= gcc
YACC= yacc
LEX	= lex

comp:	y.tab.c lex.yy.c ast.c comp.c stack.o
	$(CC) lex.yy.c y.tab.c ast.c comp.c stack.o -o comp

y.tab.c: parser.y
	$(YACC) -d parser.y

lex.yy.c: scanner.l y.tab.h
	$(LEX) scanner.l

stack.o: stack.c stack.h
	$(CC) -c stack.c

clean: 
	rm comp lex.yy.c y.tab.c y.tab.h
