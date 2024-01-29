%{
   #include <stdio.h>
   void mp(char *type);
%}

%%
-?[0-9]+ {mp("arithmetic const");}
("<"|">"|"="|"<="|">=")   {mp("arithmetic comparison");}
("+"|"-"|"*"|"div"|"mod") {mp("arithmetic operation");}
define-fun {mp("function definition keyword");}
(get-int|get-bool)  {mp("predefined function");}
(true|false)  {mp("boolean constant");}
(and|or|not)  {mp("boolean operator");}
if {mp("boolean operator");}
let   {mp("local variable declaration");}
(int|bool) {mp("variable/function type");}
print {mp("program entry point");}
[0-9]+[a-zA-Z]+[0-9a-zA-Z]*   {mp("error");}
[a-zA-Z]+[0-9a-zA-Z]*   {mp("variable/function name");}
"("   {mp("left parenthesis");}
")"   {mp("right parenthesis");}
"\n" {yylineno ++;}
;(" "|.)* {}
[ ]*  {}
. {mp("error\n");}
%%

int main(){
   yylex();
   return 0;
}

void mp(char* type) {
    printf("line %3i: %20s: ", yylineno, type);
    printf("%s\n", yytext);
}
