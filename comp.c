#include <string.h>
#include "y.tab.h"
#include "ast.h"
#include "stack.h"
#include "table.h"
int yyparse();

void visit(struct ast *node){
  printf("\tNode %d (%s) visited\n", node->id, node->token);
}

int fill_ast(struct ast *node){
  struct ast_child *child;
  char *name, *scope;
  int type, let_id, is_func;
  if (!strcmp(node->token, "funcdef")) {
    printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    name = (char *) malloc(strlen(node->child->id->token)*sizeof(char) + 1);
    strcpy(name, node->child->id->token);
    scope = (char *) malloc(5 * sizeof(char));
    strcpy(scope, "prog");
    int n = get_child_num(node) - 2;
    for (child = node->child; child && n != 0; child = child->next){ n--; }
    printf("Type: %s\n", child->id->token);
    char *tp = strchr(child->id->token, ' ') + 1;
    if (!strcmp(tp, "bool")) type = 0;
    else if (!strcmp(tp, "int")) type = 1;
    else type = -1; // TODO: Determine type
    let_id = 0;
    is_func = 1;
  } else if (!strcmp(node->token, "PEP")){
    printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    name = (char *) malloc(4*sizeof(char));
    strcpy(name, "PEP");
    scope = (char *) malloc(5 * sizeof(char));
    strcpy(scope, "prog");
    type = -1; // TODO: Determine type
    let_id = 0;
    is_func = 1;
  } else if (!strcmp(node->token, "let")) {
    printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    struct ast *definition, *parent;
    // for (parent = node->parent; parent; parent = parent->parent) visit(parent);
    child = node->child;
    definition = child->next->id;
    printf("Name: %s\n", child->id->token);
    name = (char *) malloc(strlen(child->id->token)*sizeof(char) + 1);
    strcpy(name, child->id->token);
    if (!strcmp(definition->token, "getbool")){
      type = 0;
    } else if (!strcmp(definition->token, "getint")){
      type = 1;
    } else {
      printf("Type not definite\n");
      type = -1;
    }
    for (parent = node->parent; parent->parent; parent = parent->parent){}
    if (!strcmp(parent->token, "funcdef")){
      printf("Scope: %s\n", parent->child->id->token);
      scope = (char *) malloc(strlen(parent->child->id->token) * sizeof(char) + 1);
      strcpy(scope, parent->child->id->token);
    } else if (!strcmp(parent->token, "PEP")){
      printf("Scope: PEP\n");
      scope = (char *) malloc(4 * sizeof(char));
      strcpy(scope, "PEP");
    } else {
      printf("Scope: Error\n");
    }
    // Let ID here
    let_id = 0;
    is_func = false;
  } else return 0;
    st_append(name, type, node->id, scope, let_id, is_func);
  // visit(node);
  return 0;
}

int main (int argc, char **argv) {
  extern struct stack st;
  st.top = NULL;

  extern struct sym_table table;
  table.start = NULL; 

  int retval = yyparse();
  visit_ast(fill_ast);
  st_print();
  if (retval == 0) print_ast();
  free_ast();
  return retval;
}
