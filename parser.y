%{
    #include <stdio.h>
    #include "y.tab.h"
	#include "ast.h"
	extern char *yytext;
    int yylex(void);
    void yyerror(char *msg);
%}

%start program

%token AOP MAOP COMP FUNCDEF GETINT GETBOOL BOOL BOP IF NOT LET TYPE PEP ID CONST

%%

program: '(' PEP expr ')' { insert_node("PEP", 1); }
	| '(' FUNCDEF ID decllist TYPE expr ')' program { insert_node("FUNCDEF", 1); }

decllist: 
	| '(' ID TYPE ')' decllist

expr: CONST
	| ID
	| BOOL
	| '(' GETBOOL ')'
	| '(' GETINT ')'
	| '(' NOT expr ')'
	| '(' BOP expr expr exprlist ')'
	| '(' MAOP expr expr exprlist ')'
	| '(' AOP expr expr ')'
	| '(' COMP expr expr ')'
	| '(' IF expr expr expr ')'
	| '(' ID exprlist ')'
	| '(' LET '(' ID expr ')' expr ')'

exprlist:
	| expr exprlist
%%

int yywrap() {
    return 1;
}

void yyerror(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}
