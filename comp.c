#include "y.tab.h"
#include "ast.h"
#include "stack.h"
int yyparse();

int main (int argc, char **argv) {
  extern struct stack st;
  st.top = NULL;
  int retval = yyparse();
  if (retval == 0) print_ast();
  free_ast();
  return retval;
}
