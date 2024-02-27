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
    | ID TYPE decllist

// expr: CONST

// ifs: IF

expr: term
    | fla
    | '(' MAOP term multterm ')'
    | '(' AOP term term ')'
    | '(' COMP term term ')'
    | '(' BOP fla multfla ')'
    | '(' NOT fla ')'

term: CONST
    | ID
    | '(' GETINT ')'

multterm: term
    | term multterm

fla: BOOL
    | '(' GETBOOL ')'

multfla: fla
    | fla multfla

%%

void yyerror(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}
