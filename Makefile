default: parser lexer compile

lexer: scanner.l
	lex scanner.l

parser: parser.y
	yacc -d parser.y

compile: y.tab.c y.tab.h
	gcc y.tab.c lex.yy.c -ll

clean:
	rm -rf *.c *.h