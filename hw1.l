%{
void mp();
%}

%% 

" "|"\t" { ; }
"\n" { yylineno++; }
^;.* { ; }
"\(" { mp("left parenthesis"); }
"\)" { mp("right parenthesis"); }
-?[0-9]* { mp("arith const"); }
[<>"<="">="=] { mp("arith comparison"); }
[+\-\*] { mp("arith operation"); }
(div)|(mod) { mp("arith operation"); }
(define-fun) { mp("func definition"); }
(get-int)|(get-bool) { mp("predefined func"); }
print { mp("program entry point"); }
(true)|(false) { mp("boolean const"); }
(and)|(or)|(not) { mp("boolean operator"); }
if { mp("conditional operator"); }
let { mp("local variable decl"); }
(int)|(bool) { mp("var/func type"); }
[a-zA-Z]+[a-zA-Z0-9]* { mp("var/func name"); }

%% 

int main() {
    yylex();
    return 0;
}

void mp(char* type) {
    printf("line %3i: %20s: ", yylineno, type);
    printf("%s\n", yytext);
}