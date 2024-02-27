%{
   #include <stdio.h>
   #include "y.tab.h"
   void mp(char *type);
%}

%%

-?[0-9]+                      { mp("arithmetic const"); return CONST;}
("<"|">"|"="|"<="|">=")       { mp("arithmetic comparison"); return COMP;}
("-"|"div"|"mod")             { mp("arithmetic operation"); return AOP;}
("+"|"*")                     { mp("arithmetic operation"); return MAOP;}
define-fun                    { mp("function definition keyword"); return FUNCDEF;}
get-int                       { mp("get-int func"); return GETINT;}
get-bool                      { mp("get-bool func"); return GETBOOL;}
(true|false)                  { mp("boolean constant"); return BOOL;}
(and|or)                      { mp("boolean operator"); return BOP;}
not                           { mp("boolean operator"); return NOT;}
if                            { mp("boolean operator"); return IF;}
let                           { mp("local variable declaration"); return LET;}
(int|bool)                    { mp("variable/function type"); return TYPE;}
print                         { mp("program entry point"); return PEP;}
[0-9]+[a-zA-Z]+[0-9a-zA-Z]*   { mp("error");}
[a-zA-Z]+[0-9a-zA-Z]*         { mp("variable/function name"); return ID;}
"("                           { mp("left parenthesis"); return '(';}
")"                           { mp("right parenthesis"); return ')';}
"\n"                          { yylineno++; }
;(" "|.)*                     {}
[ ]*                          {}

%%

void mp(char* type) {
    //printf("line %3i: %30s: ", yylineno, type);
    //printf("%s\n", yytext);
}
