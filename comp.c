#include <string.h>
#include "y.tab.h"
#include "ast.h"
#include "stack.h"
#include "table.h"
#include "visitors.h"
#include "map.h"
int yyparse();

int main (int argc, char **argv) {
  extern struct stack st;
  st.top = NULL;

  extern struct sym_table table;
  table.start = NULL; 

  extern struct type_map map;
  map.start = NULL;

  int retval = yyparse();
  if (retval == 0) {
    visit_ast(fill_table);
    if (visit_ast(declare_var_before_use)) return 1;
    if (visit_ast(declare_func_before_use)) return 2;
    if (visit_ast(match_num_args_func)) return 3;
    if (visit_ast(unique_func_names)) return 4;
    if (visit_ast(vars_with_func_names)) return 5;
    if (visit_ast(duplicate_var_declare)) return 6;
    if (visit_ast(duplicate_arg_func)) return 7;

    visit_ast(init_map);
    //tm_print();
    // pass over as many times as needed until zero unknowns remain
    
    while (visit_ast(fill_map) == 0) {
      printf("passed\n");
    }
    
    //visit_ast(fill_map);
    tm_print();

    st_print();  // should not print if any failures occured
    print_ast(); 

    visit_ast(well_formed_aop);
    visit_ast(well_formed_bop);
  }
  free_ast();
  st_free();
  tm_free();
  return retval;
}
