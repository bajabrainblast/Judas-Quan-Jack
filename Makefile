CC	= gcc
YACC= yacc
LEX	= lex

comp:	y.tab.c lex.yy.c ast.c comp.c
	$(CC) lex.yy.c y.tab.c ast.c comp.c -o comp

y.tab.c: parser.y
	$(YACC) -d parser.y

lex.yy.c: scanner.l y.tab.h
	$(LEX) scanner.l

clean: 
	rm comp lex.yy.c y.tab.c y.tab.h
