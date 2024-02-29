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
   void insert_pass_through(int i);
%}

%start program

%union {int val; char* str;}

%token<str> FUNCDEF GETINT GETBOOL BOP IF NOT LET TYPE PEP ID BOOL CONST AOP MAOP COMP

%type<val> program decllist_base expr exprlist const id bool type

%%

id : ID { $$ = insert_node($1,0);}

const : CONST { $$ = insert_node($1,0);}

bool : BOOL { $$ = insert_node($1,0);}

type : TYPE { $$ = insert_node($1,0);}

program: '(' PEP expr ')' 									{ insert_child($3);
																	  insert_node("PEP", 0);
																	  pt(1); }
	| '(' FUNCDEF id decllist_base type expr ')' program { insert_child($3);
                                                     insert_pass_through($4);
																	  insert_child($5);
																	  insert_child($6);
																	  insert_node("funcdef", 0);
																	  pt(2); }
decllist_base: decllist       { $$ = insert_node("decllist_base",0);}

decllist:							{ pt(3.1); }
	| '(' id type ')' decllist {  insert_child($2);
										  pt(3.2); }

expr: const 									{ $$ = $1;
													  pt(4); }
	| id 											{ $$ = $1;
	       	  									  pt(5); }
	| bool 										{ $$ = $1;
              									  pt(6); }
	| '(' GETBOOL ')' 						{ $$ = insert_node("getbool",0);
													  pt(7); }
	| '(' GETINT ')' 							{ $$ = insert_node("getint",0);
													  pt(8); }
	| '(' NOT expr ')'						{ insert_child($3);
													  $$ = insert_node("not",0);
													  pt(9); }
	| '(' BOP expr expr exprlist ')'		{ insert_child($3);
													  insert_child($4);
													  $$ = insert_node($2,0);
													  pt(10); }
	| '(' MAOP expr expr exprlist ')'	{ insert_child($3);
													  insert_child($4);
													  $$ = insert_node($2,0);
													  pt(11); }
	| '(' AOP expr expr ')' 				{ insert_child($3);
													  insert_child($4);
													  $$ = insert_node($2,0);
													  pt(12); }
	| '(' COMP expr expr ')' 				{ insert_child($3);
													  insert_child($4);
													  $$ = insert_node($2,0);
													  pt(13); }
	| '(' IF expr expr expr ')' 			{ insert_child($3);
													  insert_child($4);
													  insert_child($5);
													  $$ = insert_node("if",0);
													  pt(14); }
	| '(' ID exprlist ')' 					{ $$ = insert_node($2,0);
													  pt(15); }
	| '(' LET '(' id expr ')' expr ')' 	{ insert_child($4);
	                                      insert_child($5);
													  insert_child($7);
													  $$ = insert_node("let",0);
													  pt(16); }

exprlist:				{ pt(17.1); }
	| expr exprlist	{ insert_child($1);
							  pt(17.2); }

%%

void insert_pass_through(int i) {
   extern struct ast *ast_list_root;
   struct ast *node = find_ast_node(i);
   struct ast *prev = ast_list_root;
   while (prev != NULL && prev->next != node) {
      prev = prev->next;
   }
   struct ast_child *ptr = node->child;
   while (ptr != NULL && ptr->next != NULL) {
      struct ast_child *old_ptr = ptr;
      insert_child(ptr->id->id);
      ptr = ptr->next;
      free(old_ptr->id);
      free(old_ptr);
   }
   ast_child_root = NULL;
   if (ast_list_root == node) {
      ast_list_root = node->next; 
   }
   if (prev != NULL)
      prev->next = node->next;
   free(node);
}
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
