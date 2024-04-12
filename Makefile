CC	= gcc
YACC= yacc
LEX	= lex

comp:	y.tab.c lex.yy.c ast.c comp.c stack.o table.o helpers.o visitors.o map.o cfg.o
	$(CC) -g lex.yy.c y.tab.c ast.c comp.c stack.o table.o helpers.o visitors.o map.o cfg.o -o comp

y.tab.c: parser.y
	$(YACC) -d parser.y

lex.yy.c: scanner.l y.tab.h
	$(LEX) scanner.l

stack.o: stack.c stack.h
	$(CC) -c -g stack.c

table.o: table.c table.h
	$(CC) -c -g table.c

helpers.o: helpers.c helpers.h
	$(CC) -c -g helpers.c

visitors.o: visitors.c visitors.h
	$(CC) -c -g visitors.c

map.o: map.c map.h
	$(CC) -c -g map.c

cfg.o: cfg.c cfg.h
	$(CC) -c -g cfg.c

clean: 
	rm -f comp lex.yy.c y.tab.c y.tab.h *.o
