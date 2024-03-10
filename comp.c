#include <string.h>
#include "y.tab.h"
#include "ast.h"
#include "stack.h"
#include "table.h"
int yyparse();

int isArithematic(char *op){
  return !strcmp(op, "+") || !strcmp(op, "-") || !strcmp(op, "*") ||
         !strcmp(op, "div") || !strcmp(op, "mod") || !strcmp(op, "getint");
}

int isBoolean(char *op){
  return !strcmp(op, "=") || !strcmp(op, "<") || !strcmp(op, ">") ||
         !strcmp(op, "<=") || !strcmp(op, ">=") || !strcmp(op, "not") ||
         !strcmp(op, "and") || !strcmp(op, "or")|| !strcmp(op, "getbool");
}

int getType(struct ast *node){
  printf("%s\n", node->token);
  if (isBoolean(node->token)) return 0;
  if (isArithematic(node->token)) return 1;
  if (!strcmp(node->token, "let")) return getType(node->child->next->id);
  if (!strcmp(node->token, "if")) return getType(node->child->next->id);
  return st_get_type(node);
}

void visit(struct ast *node){
  printf("\tNode %d (%s) visited\n", node->id, node->token);
}

int fill_table(struct ast *node){
  struct ast_child *child;
  char *name, *scope;
  int args[10];
  int types[10];
  int num_arg = 0;
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
    char *tp = strchr(child->id->token, ' ') + 1;
    if (!strcmp(tp, "bool")) type = 0;
    else if (!strcmp(tp, "int")) type = 1;
    else type = -1;
    let_id = 0;
    is_func = 1;
    int end_arg = get_child_num(node) - 2;
    int start_arg = 1;
    child = node->child->next;
    for (; start_arg < end_arg; start_arg += 2) {
       if (strcmp(child->id->token,"bool")) {
          types[num_arg] = 0;
       }
       else {
          types[num_arg] = 1;
       }
       child = child->next;
       args[num_arg] = child->id->id;
       child = child->next;
       printf("%d\n",num_arg);
       num_arg ++;
    }
  } else if (!strcmp(node->token, "PEP")){
    printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    name = (char *) malloc(4*sizeof(char));
    strcpy(name, "PEP");
    scope = (char *) malloc(5 * sizeof(char));
    strcpy(scope, "prog");
    type = getType(node->child->id); // TODO: Determine type
    let_id = 0;
    is_func = 1;
  } else if (!strcmp(node->token, "let")) {
    printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    struct ast *definition, *parent;
    // for (parent = node->parent; parent; parent = parent->parent) visit(parent);
    definition = node->child->next->id;
    if (!strcmp(definition->token, "getbool")) type = 0;
    else if (!strcmp(definition->token, "getint")) type = 1; 
    else type = getType(definition);
    // else {
    //   child = definition->child;
    //   if (isBoolean(child->id->token)) type = 0;
    //   else if (isArithematic(child->id->token)) type = 1;
    //   if ()
    //   type = getType(child->)
    // }
    child = node->child;
    name = (char *) malloc(strlen(child->id->token)*sizeof(char) + 1);
    strcpy(name, child->id->token);
    for (parent = node->parent; parent->parent; parent = parent->parent){}
    if (!strcmp(parent->token, "funcdef")){
      scope = (char *) malloc(strlen(parent->child->id->token) * sizeof(char) + 1);
      strcpy(scope, parent->child->id->token);
    } else if (!strcmp(parent->token, "PEP")){
      scope = (char *) malloc(4 * sizeof(char));
      strcpy(scope, "PEP");
    } else {
      printf("Scope: Error\n");
    }
    let_id = 0;
    is_func = false;
  } else return 0;
    st_append(name, type, node->id, scope, let_id, args, types, num_arg, is_func);
  return 0;
}

int main (int argc, char **argv) {
  extern struct stack st;
  st.top = NULL;

  extern struct sym_table table;
  table.start = NULL; 

  int retval = yyparse();
  visit_ast(fill_table);
  st_print();
  if (retval == 0) print_ast();
  free_ast();
  return retval;
}
