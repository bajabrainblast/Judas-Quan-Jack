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
  if (retval == 0) {
    visit_ast(fill_table);
    retval = visit_ast(declare_var_before_use);
    if (retval) printf("");
    retval = visit_ast(declare_func_before_use);
    if (retval) printf("");
    retval = visit_ast(match_num_args_func);
    if (retval) printf("");
    retval = visit_ast(unique_func_names);
    if (retval) printf("");
    retval = visit_ast(vars_with_func_names);
    if (retval) printf("");
    retval = visit_ast(duplicate_var_declare);
    if (retval) printf("");
    retval = visit_ast(duplicate_arg_func);
    if (retval) printf("");
    st_print();  // should not print if any failures occured
    print_ast(); 
  }
  free_ast();
  st_free();
  return retval;
}
