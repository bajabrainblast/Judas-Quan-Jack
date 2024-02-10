%{
    #include <stdio.h>
    #include "y.tab.h"
    int yylex(void);
    void yyerror(char *msg);
%}

%start program

%token AOP MAOP COMP FUNCDEF GETINT GETBOOL BOOL BOP IF NOT LET TYPE PEP ID CONST

%%

program: expr
		| program expr

expr: CONST

multexpr:
        | multexpr expr

term: CONST
    | ID
    | GETINT
    | (MAOP term multterm)
    | (AOP term term)
    | (IF fla term term)
    | (ID multexpr)
    | (LET (ID expr) term)

multterm: term 
        | term multterm
multfla : fla 
	| fla mfla 
fla : BOOL
	| ID
	| '(' GETBOOL ')'
	| '(' COMP term term ')'
	| '(' NOT fla ')'
	| '(' BOP fla multfla ')'
	| '(' IF fla fla fla ')'
	| '(' ID expr ')'
	| '(' LET '(' ID expr ')' fla ')'

%%

int main(void){
    yyparse();
    return 0;
}

void yyerror(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}
