#include "y.tab.h"
#include "ast.h"
#include "stack.h"
#include "table.h"
int yyparse();

void visitNode(struct ast *node){
  if (!node) return;
  printf("%d: %s\n", node->id, node->token);
  struct ast_child *child;
  for (child = node->child; child && child->id; child = child->next)
    visitNode(child->id);
}

int main (int argc, char **argv) {
  extern struct stack st;
  st.top = NULL;

  extern struct sym_table table;
  table.start = NULL;

  /*
  st_append("x", 0, "prog", 0, 0);
  st_append("y", 0, "prog", 0, 0);
  st_append("z", 0, "prog", 0, 0);
  st_append("myFunc", 1, "prog", 0, 1);
  st_append("a", 1, "myFunc", 0, 0);
  st_append("b", 1, "myFunc", 0, 0);
  st_append("c", 1, "myFunc", 0, 0);
  st_append("otherFunc", 0, "prog", 0, 1);

  printf("does q appear in the st? %i\n", st_exists("q"));
  printf("does x appear in the st? %i\n", st_exists("x"));
  printf("does x appear in myFunc? %i\n", st_exists_in("x", "myFunc"));
  printf("does a appear in myFunc? %i\n", st_exists_in("a", "myFunc"));
  printf("does a appear in otherFunc? %i\n", st_exists_in("x", "otherFunc"));
  st_print();
  */

  int retval = yyparse();
  if (retval == 0) print_ast();
  if (!ast_list_root) printf("NO ROOT!\n");
  visitNode(ast_list_root);
  free_ast();
  return retval;
}
