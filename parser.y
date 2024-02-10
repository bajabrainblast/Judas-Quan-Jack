%{
    #include <stdio.h>
    #include "y.tab.h"
    int yylex(void);
    void yyerror(char *msg);
%}

%start program

%token AOP COMP FUNCDEF PREDEFFUNC BOOL BOP IF LET TYPE PEP ID CONST

%%

program: expr
		| program expr

expr: CONST

%%

int main(void){
    yyparse();
    return 0;
}

void yyerror(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}