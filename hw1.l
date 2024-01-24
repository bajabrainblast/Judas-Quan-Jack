%{
#include <stdio.h>
int count = 1;
void myPrint();
void lineSkip();
%}

%% 

" " { ; }
^; { lineSkip(); }
"\(" { myPrint(); }
"\)" { myPrint(); }
"\n" { count++; }
[^()"\n"" "]+ { myPrint(); }


%% 

int main() {
    yylex();
    return 0;
}

void myPrint() {
    printf("line %i: ", count);
    printf("TYPE: %.*s\n", yyleng, yytext);
    return;
}

void lineSkip() {
    while(input() != '\n');
    return;
}