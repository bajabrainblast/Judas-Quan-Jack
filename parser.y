%{
	/* TO DO
		need to extract current token and use it instead of placeholder strings for node insertion
		need to test all productions
		possible special case for let? 
	*/

    #include <stdio.h>
    #include "y.tab.h"
	#include "ast.h"
    #include "stack.h"

	extern char *yytext;
    extern struct stack st;

    int yylex(void);
    void yyerror(char *msg);

	int extra = 0;
	void pt(int);				// simple printing func for tracing execution
	void get_children(int i);	// performs i many of this operation: pop the top id from stack, make that node a child 
%}

%start program

%token AOP MAOP COMP FUNCDEF GETINT GETBOOL BOOL BOP IF NOT LET TYPE PEP ID CONST

%%

program: '(' PEP expr ')' 				{ insert_child(pop(&st)); insert_node("PEP", 0); pt(1); }
	| '(' FUNCDEF ID decllist TYPE expr ')' program { insert_node("FUNCDEF", 0); pt(2); }

decllist: 
	| '(' ID TYPE ')' decllist 			{ insert_child(insert_node("decllist", 0)); pt(3); }

expr: CONST 							{ push(&st,insert_node("const",0)); pt(4); }
	| ID 								{ push(&st,insert_node("id",0)); pt(5); }
	| BOOL 								{ push(&st,insert_node("bool",0)); pt(6); }
	| '(' GETBOOL ')' 					{ push(&st,insert_node("getbool",0)); pt(7); }
	| '(' GETINT ')' 					{ push(&st,insert_node("getint",0)); pt(8); }
	| '(' NOT expr ')'					{ insert_child(pop(&st)); insert_node("not",0); pt(9); }
	| '(' BOP expr expr exprlist ')'	{ insert_child(pop(&st)); insert_child(pop(&st)); push(&st,insert_node("bop",0)); pt(10); }
	| '(' MAOP expr expr exprlist ')'	{ insert_child(pop(&st)); insert_child(pop(&st)); push(&st,insert_node("maop",0)); pt(11); }
	| '(' AOP expr expr ')' 			{ insert_child(pop(&st)); insert_child(pop(&st)); push(&st,insert_node("aop",0)); pt(12); }
	| '(' COMP expr expr ')' 			{ insert_child(pop(&st)); insert_child(pop(&st)); push(&st,insert_node("comp",0)); pt(13); }
	| '(' IF expr expr expr ')' 		{ insert_child(pop(&st)); insert_child(pop(&st)); insert_child(pop(&st)); push(&st,insert_node("if",0)); pt(14); } 
	| '(' ID exprlist ')' 				{ push(&st,insert_node("id",0)); pt(15); }
	| '(' LET '(' ID expr ')' expr ')' 	{ int id_id = insert_node("id",0); insert_child(id_id); insert_child(pop(&st)); insert_child(pop(&st)); push(&st,insert_node("let",0)); pt(16); }

exprlist:
	| expr exprlist 					{ insert_child(pop(&st)); pt(17); }
%%

int yywrap() {
    return 1;
}

void yyerror(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
}

// comment out the print if unnecessary
void pt(int i) {
	printf("%i\n", i);
}

void get_children(int i) {
	int j;
	for (j=0; j<i; j++) {
		insert_child(pop(&st));
	}
}
