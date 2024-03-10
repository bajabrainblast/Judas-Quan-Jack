#include "helpers.h"
#include "visitors.h"

void visit(struct ast *node){
  printf("\tNode %d (%s) visited\n", node->id, node->token);
}

int fill_table(struct ast *node){
  struct ast_child *child;
  char name[50];
  char scope[50];
  struct arg args[10];
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
          args[num_arg].type = 0;
       }
       else {
          args[num_arg].type = 1;
       }
       child = child->next;
       args[num_arg].id = child->id->id;
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
    st_append(name, type, node->child->id->id, scope, let_id, args, num_arg, is_func);
    args[0].id = node->child->id->id;
    args[0].type = type;
    num_arg = 1;
    strcpy(name, node->token);
  } else return 0;
  st_append(name, type, node->id, scope, let_id, args, num_arg, is_func);
  for (int i = 0; i < num_arg; i++){
    args[i].id = 0;
    args[i].type = -1;
  }
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
               if (strcmp(find_ast_node(en->args[i].id)->token,node->token) == 0) {
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

int declare_func_before_use(struct ast *node) {
   if (node->ntoken == 2) {
      //printf("%s is var\n",node->token);
      struct ast *tmp = node->parent;
      struct table_entry *en = st_find_entry(node->token,"prog");
      if (en == NULL) {
         printf("Function %s not declared\n",node->token);
         return 1;
      }
      struct ast *n = find_ast_node(en->node_id);
      int use_id = node->id;
      printf("node id %d\n",use_id);
      int decl_id = en->node_id;
      printf("decl id %d\n",decl_id);
      if (decl_id < use_id) {
         printf("Function %s not declared before use\n",node->token);
         return 1;
      }
      else {
         printf("Function %s has benn declared correctly\n",node->token);
         return 0;
      }
   }

   return 0;
}
int unique_func_names() {
  extern struct sym_table table;
  struct table_entry *en, *en2;

  for (en = table.start; en != NULL; en = en->next) {
    // for each func
    if (en->is_func == 1) {
      for (en2 = en->next; en2 != NULL; en2 = en2->next) {
        if (en2->is_func == 1) {
          // compare against all other funcs
          if (strcmp(en->name, en2->name) == 0)
            return 1; // not unique
        }
      }
    }
  }
  return 0;
}

int vars_with_func_names() {
  extern struct sym_table table;
  struct table_entry *en, *en2;

  for (en = table.start; en != NULL; en = en->next) {
    // for each var
    if (en->is_func == 0) {
      for (en2 = en->next; en2 != NULL; en2 = en2->next) {
        if (en2->is_func == 1) {
          // compare against all other funcs
          if (strcmp(en->name, en2->name) == 0)
            return 1; // not unique
        }
      }
    }
  }
  return 0;
}

int unique_vars_in_scope(){
  return 1;
}