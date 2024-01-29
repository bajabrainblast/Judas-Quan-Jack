all:
	lex hw1.l && gcc lex.yy.c -ll
