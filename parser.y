%{
    #include <stdio.h>
    #include "y.tab.h"
    int yylex(void);
    void yyerror(char *msg);
%}

%start program

%token AOP COMP FUNCDEF GETINT GETBOOL BOOL BOP IF LET TYPE PEP ID CONST

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

%%

int main(void){
    yyparse();
    return 0;
}

void yyerror(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}
