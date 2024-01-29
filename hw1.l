%{
   #include <stdio.h>
%}

%%
-?[0-9]+ {printf("line %d: arithmetic const: %s\n",yylineno,yytext);}
("<"|">"|"="|"<="|">=")   {printf("line %d: arithmetic comparison: %s\n",yylineno,yytext);}
("+"|"-"|"*"|"div"|"mod") {printf("line %d: arithmetic operation: %s\n",yylineno,yytext);}
define-func {printf("line %d: function definition keyword: %s\n",yylineno,yytext);}
(get-int|get-bool)  {printf("line %d: predefined function: %s\n",yylineno,yytext);}
(true|false)  {printf("line %d: boolean constant: %s\n",yylineno,yytext);}
(and|or|not)  {printf("line %d: boolean operator: %s\n",yylineno,yytext);}
if {printf("line %d: boolean operator: %s\n",yylineno,yytext);}
let   {printf("line %d: local variable declaration: %s\n",yylineno,yytext);}
(int|bool) {printf("line %d: variable/function type: %s\n",yylineno,yytext);}
print {printf("line %d: program entry point: %s\n",yylineno,yytext);}
[0-9]+[a-zA-Z]+[0-9a-zA-Z]*   {printf("error\n");}
[a-zA-Z]+[0-9a-zA-Z]*   {printf("line %d: variable/function name: %s\n",yylineno,yytext);}
"("   {printf("line %d: left parenthesis: %s\n",yylineno,yytext);}
")"   {printf("line %d: right parenthesis: %s\n",yylineno,yytext);}
"\n" {yylineno ++;}
;(" "|.)* {}
[ ]*  {}
. {printf("error\n");}
%%

int main(){
   yylex();
   return 0;
}
