#include "helpers.h"
#include "visitors.h"

void visit(struct ast *node){
  printf("\tNode %d (%s) visited\n", node->id, node->token);
}

int fill_table(struct ast *node){
  struct ast_child *child;
  char name[50];
  char scope[50];
  int args[10];
  int types[10];
  int num_arg = 0;
  int type, let_id, is_func;
  if (!strcmp(node->token, "funcdef")) {
    printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    strcpy(name, node->child->id->token);
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
    strcpy(name, "PEP");
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
    strcpy(name, child->id->token);
    for (parent = node->parent; parent->parent; parent = parent->parent){}
    if (!strcmp(parent->token, "funcdef")){
      strcpy(scope, parent->child->id->token);
    } else if (!strcmp(parent->token, "PEP")){
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

int declare_var_before_use(struct ast *node) {
   if (node->ntoken == 1) {
      //printf("%s is var\n",node->token);
      struct ast *tmp = node->parent;
      while (tmp != NULL) {
         //printf("explore %s\n",tmp->token);
         if (strcmp(tmp->token,"funcdef") == 0) {
            struct table_entry *en = st_find_entry(tmp->child->id->token,"prog");
            if (en == NULL) {
               printf("Variable %s not declared\n",node->token);
               return 1;
            }
            int num_arg = en->num_arg;
            int i;
            for (i = 0; i < num_arg; i ++) {
               if (strcmp(find_ast_node(en->args[i])->token,node->token) == 0) {
                  printf("Variable %s is good\n",node->token);
                  return 0;
               }
            }
            printf("Variable %s not declared\n",node->token);
            return 1;
         }
         tmp = tmp->parent;
      }
   }

   return 0;
}
