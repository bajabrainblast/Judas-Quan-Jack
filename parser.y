%{
    #include <stdio.h>
    #include "y.tab.h"
    int yylex(void);
    void yyerror(char *msg);
%}

%start program

%token AOP MAOP COMP FUNCDEF GETINT GETBOOL BOOL BOP IF NOT LET TYPE PEP ID CONST

%%

program: '(' PEP expr ')'
	| '(' FUNCDEF ID decllist TYPE expr ')' program

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

int main(void){
    yyparse();
    return 0;
}

void yyerror(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}
