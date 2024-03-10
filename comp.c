#include <string.h>
#include "y.tab.h"
#include "ast.h"
#include "stack.h"
#include "table.h"
#include "visitors.h"
int yyparse();

int main (int argc, char **argv) {
  extern struct stack st;
  st.top = NULL;

  extern struct sym_table table;
  table.start = NULL; 

  int retval = yyparse();
  visit_ast(fill_table);
  visit_ast(declare_var_before_use);
  st_print();
  if (retval == 0) print_ast();
  free_ast();
  st_free();
  return retval;
}
