#include <string.h>
#include "y.tab.h"
#include "ast.h"
#include "cfg.h"
#include "helpers.h"
#include "stack.h"
#include "table.h"
#include "visitors.h"
#include "map.h"
#include "codegen.h"

int yyparse();

int cleanup(int error){
  free_ast();
  st_free();
  tm_free();
  cfg_destroy();
  return error;
}

int main (int argc, char **argv) {
  int swo = 0;
  extern struct stack st;
  st.top = NULL;
  extern struct sym_table table;
  table.start = NULL;
  extern struct type_map map;
  map.start = NULL;
  for (int i = 1; i < argc; i++){
    if (!strcmp(argv[i], "--opt") || !strcmp(argv[i], "-o")) swo = 1;
  }
  if (!yyparse()) {
    visit_ast(fill_table);
    if (visit_ast(declare_var_before_use)) return cleanup(1);
    if (visit_ast(declare_func_before_use)) return cleanup(2);
    if (visit_ast(match_num_args_func)) return cleanup(3);
    if (visit_ast(unique_func_names)) return cleanup(4);
    if (visit_ast(vars_with_func_names)) return cleanup(5);
    if (visit_ast(duplicate_var_declare)) return cleanup(6);
    if (visit_ast(duplicate_arg_func)) return cleanup(7);
    visit_ast(init_map);
    while (true) {
      visit_ast(fill_map);
      if (!tm_contains_unknowns()) {
         break;
      }
    }
    print_ast();
    if (visit_ast(check_ifs)) return cleanup(14);
    if (visit_ast(check_function_returns)) return cleanup(8);
    if (visit_ast(well_formed_aop)) return cleanup(9);
    if (visit_ast(check_function_returns)) return cleanup(10);
    if (visit_ast(well_formed_bop)) return cleanup(11);
    if (visit_ast(well_formed_not)) return cleanup(14);
    if (visit_ast(if_first_arg)) return cleanup(12);
    if (visit_ast(func_call_args_type)) return cleanup(14);
  } else return 15;
  visit_ast(cfg_construct);
  cfg_dot("unopt_cfg");
  if (swo){
    // do optimizations
    int changes = 1;
    while (changes){
      changes = 0;
      merge_blocks(&changes);
      eliminate_unreachable_code(&changes);
      duplicate_branch_elimination(&changes);
      changes ? printf("%d changes\n", changes) : printf("Optimizations Done\n");
    }
    cfg_dot("cfg");
  }

  generate_c_code();

  return cleanup(0);
}
