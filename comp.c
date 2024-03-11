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
    visit_ast(declare_var_before_use);
    visit_ast(declare_func_before_use);
    visit_ast(match_num_args_func);
    visit_ast(duplicate_var_declare);

    retval = unique_func_names();
    if (retval == 0)
      printf("unique func names SUCCESS\n");
    else 
      printf("not unique func names FAILURE\n");

    retval = vars_with_func_names();
    if (retval == 0) 
      printf("vars do not have func names SUCCESS\n");
    else
      printf("vars have func names FAILURE\n");
    st_print();  // should not print if any failures occured
    print_ast(); 
  }
  free_ast();
  st_free();
  return retval;
}
