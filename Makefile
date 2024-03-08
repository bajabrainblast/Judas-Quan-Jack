CC	= gcc
YACC= yacc
LEX	= lex

# stack.o not in use?

comp:	y.tab.c lex.yy.c ast.c comp.c stack.o table.o
	$(CC) -g lex.yy.c y.tab.c ast.c comp.c stack.o table.o -o comp

y.tab.c: parser.y
	$(YACC) -d parser.y

lex.yy.c: scanner.l y.tab.h
	$(LEX) scanner.l

stack.o: stack.c stack.h
	$(CC) -c stack.c

table.o: table.c table.h
	$(CC) -c table.c

clean:
	rm comp lex.yy.c y.tab.c y.tab.h stack.o table.o
