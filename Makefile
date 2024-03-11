CC	= gcc
YACC= yacc
LEX	= lex

# stack.o not in use?

comp:	y.tab.c lex.yy.c ast.c comp.c stack.o table.o helpers.o visitors.o
	$(CC) -g lex.yy.c y.tab.c ast.c comp.c stack.o table.o helpers.o visitors.o -o comp

y.tab.c: parser.y
	$(YACC) -d parser.y

lex.yy.c: scanner.l y.tab.h
	$(LEX) scanner.l

stack.o: stack.c stack.h
	$(CC) -c stack.c

table.o: table.c table.h
	$(CC) -c table.c

helpers.o: helpers.c helpers.h
	$(CC) -c helpers.c

visitors.o: visitors.c visitors.h
	$(CC) -c visitors.c

clean: 
	rm comp lex.yy.c y.tab.c y.tab.h *.o
