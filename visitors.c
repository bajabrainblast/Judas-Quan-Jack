#include "visitors.h"
#include "helpers.h"
#include "table.h"
#include "map.h"

int fill_table(struct ast *node){
  struct ast_child *child;
  char name[50];
  char scope[50];
  struct arg args[10];
  int num_arg = 0;
  int type, let_id, is_func;
  if (!strcmp(node->token, "funcdef")) {
    // printf("Node %d: %s\n", node->id, node->token);
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
       if (strcmp(child->id->token,"bool") == 0) {
          args[num_arg].type = 0;
       }
       if (strcmp(child->id->token,"int") == 0){
          args[num_arg].type = 1;
       }
       child = child->next;
       args[num_arg].id = child->id->id;
       child = child->next;
       num_arg ++;
    }
  } else if (!strcmp(node->token, "PEP")){
    // printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    strcpy(name, "PEP");
    strcpy(scope, "prog");
    type = getType(node->child->id); // TODO: Determine type
    let_id = 0;
    is_func = 1;
  } else if (!strcmp(node->token, "let")) {
    // printf("Node %d: %s\n", node->id, node->token);
    // for (child = node->child; child; child = child->next) visit(child->id);
    struct ast *definition, *parent;
    // for (parent = node->parent; parent; parent = parent->parent) visit(parent);
    definition = node->child->next->id;
    if (!strcmp(definition->token, "getbool")) type = 0;
    else if (!strcmp(definition->token, "getint")) type = 1; 
    else type = getType(node->child->next->next->id);
    child = node->child;
    strcpy(name, child->id->token);
    for (parent = node->parent; parent->parent; parent = parent->parent){}
    if (!strcmp(parent->token, "funcdef")){
      strcpy(scope, parent->child->id->token);
    } else if (!strcmp(parent->token, "PEP")){
      strcpy(scope, "PEP");
    }
    let_id = 0;
    is_func = false;
    st_append(name, type, node->child->id->id, scope, let_id, args, num_arg, is_func);
    strcpy(name, node->token);
    args[0].id = node->child->id->id;
    args[0].type = type;
    num_arg = 1;
    findNestedLetVars(node->child->next->id, node->child->id->token, node->child->id->id);
    findNestedLetVars(node->child->next->next->id, node->child->id->token, node->child->id->id);
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
               // printf("Variable %s not declared FAIL\n",node->token);
               printf("Error: Variable %s not declared\n",node->token);
               return 1;
            }
            int num_arg = en->num_arg;
            int i;
            for (i = 0; i < num_arg; i ++) {
               if (strcmp(find_ast_node(en->args[i].id)->token,node->token) == 0) {
                  // printf("Variable %s is declared before use SUCCESS\n",node->token);
                  return 0;
               }
            }
            // printf("Variable %s not declared FAIL\n",node->token);
            printf("Error: Variable %s not declared\n",node->token);
            return 1;
         }
         if (strcmp(tmp->token,"let") == 0) {
            struct table_entry *en = st_get_entry(tmp->token,tmp->id);
            int num_arg = en->num_arg;
            int i;

            struct ast *let_define_node = find_ast_node(en->node_id)->child->next->id;

            if (find_parent(node,let_define_node) != NULL) {
               tmp = tmp->parent;
               continue;
            }
            for (i = 0; i < num_arg; i ++) {
               if (strcmp(find_ast_node(en->args[i].id)->token,node->token) == 0) {
                  // printf("Variable %s is declared before use SUCCESS\n",node->token);
                  return 0;
               }
            }
         }
         tmp = tmp->parent;
      }
      // printf("Variable %s not declared FAIL\n",node->token);
      printf("Error: Variable %s not declared\n",node->token);
      return 1;
   }

   return 0;
}

int duplicate_arg_func(struct ast *node) {
   if (strcmp(node->token,"funcdef") == 0) {
      char *func_name = node->child->id->token;
      struct table_entry *en = st_get_entry(func_name,node->id);
      int num_arg = en->num_arg;
      int i;
      int j;
      for (i = 0; i < num_arg; i ++) {
         int count = 0;
         for (j = 0; j < num_arg; j ++) {
            if (strcmp(find_ast_node(en->args[i].id)->token
                       ,find_ast_node(en->args[j].id)->token) == 0) {
               count ++;
            }
         }
         if (count >= 2) {
            // printf("Duplicate arg %s in function %s FAIL\n",find_ast_node(en->args[i].id)->token,func_name);
            printf("Error: Function argument %s defined twice\n",find_ast_node(en->args[i].id)->token);
            return 1;
         }
      }
      // printf("No duplicate in function %s SUCCESS\n",func_name);
      return 0;
   }
   return 0;
}

int duplicate_var_declare(struct ast *node) {
   if (node->ntoken == 3) {
      //printf("%s is var\n",node->token);
      struct ast *tmp = node->parent;
      struct table_entry *let_entry = st_get_entry("let",node->id);
      char *var_decl = find_ast_node(let_entry->args[0].id)->token;
      while (tmp != NULL) {
         //printf("explore %s\n",tmp->token);
         if (strcmp(tmp->token,"funcdef") == 0) {
            struct table_entry *en = st_find_entry(tmp->child->id->token,"prog");
            if (en == NULL) {
               // printf("Variable %s not declared FAIL\n",var_decl);
               printf("Error: Variable %s declared twice\n",var_decl);
               return 1;
            }
            int num_arg = en->num_arg;
            int i;
            for (i = 0; i < num_arg; i ++) {
               if (strcmp(find_ast_node(en->args[i].id)->token,var_decl) == 0) {
                  // printf("Variable %s is duplicate FAIL\n",var_decl);
                  printf("Error: Variable %s declared twice\n",var_decl);
                  return 1;
               }
            }
            // printf("Variable %s is NOT duplicate SUCCESS\n",var_decl);
            return 0;
         }
         if (strcmp(tmp->token,"let") == 0) {
            struct table_entry *en = st_get_entry(tmp->token,tmp->id);
            int num_arg = en->num_arg;
            int i;
	    if (strcmp(find_ast_node(en->args[0].id)->token,var_decl) == 0) {
		// printf("Variable %s is duplicate FAIL\n",var_decl);
      printf("Error: Variable %s declared twice\n",var_decl);
		return 1;
	    }
         }
         tmp = tmp->parent;
      }
      // printf("Variable %s is NOT duplicate SUCCESS\n",var_decl);
      return 0;
   }

   return 0;
}

int declare_func_before_use(struct ast *node) {
   if (node->ntoken == 2) {
      //printf("%s is var\n",node->token);
      struct ast *tmp = node->parent;
      struct table_entry *en = st_find_entry(node->token,"prog");
      if (en == NULL) {
         // printf("Function %s not declared FAIL\n",node->token);
         printf("Error: Function %s not defined\n",node->token);
         return 1;
      }
      struct ast *n = find_ast_node(en->node_id);
      int use_id = node->id;
      //printf("node id %d\n",use_id);
      int decl_id = en->node_id;
      //printf("decl id %d\n",decl_id);
      if (decl_id < use_id) {
         // printf("Function %s not declared before use FAIL\n",node->token);
         printf("Error: Function %s not defined\n",node->token);
         return 1;
      }
      else {
         // printf("Function %s has been declared before use SUCCESS\n",node->token);
         return 0;
      }
   }

   return 0;
}

int unique_func_names(struct ast *node) {
  struct table_entry *f = st_get_func(node->token);
  if (!f) return 0;
  if (st_is_func_unique(f->name)) {
      printf("Error: Function %s name defined twice\n",node->token);
      return 1;
  }
  return 0;
  // extern struct sym_table table;
  // struct table_entry *en, *en2;

  // for (en = table.start; en != NULL; en = en->next) {
    // for each func
  //   if (en->is_func == 1) {
  //     for (en2 = en->next; en2 != NULL; en2 = en2->next) {
  //       if (en2->is_func == 1) {
  //         // compare against all other funcs
  //         if (strcmp(en->name, en2->name) == 0)
  //           return 1; // not unique
  //       }
  //     }
  //   }
  // }
  // return 0;
}

int vars_with_func_names(struct ast *node) {
  if (node->ntoken == 1 && st_get_func(node->token)){
    struct table_entry *f = st_get_func(node->token);
   //  printf("%p\n", f);
   printf("Error: Variable shares a name of a defined function %s\n",f->name);
    return 1;
  }
  return 0;
  // extern struct sym_table table;
  // struct table_entry *en, *en2;

  // for (en = table.start; en != NULL; en = en->next) {
  //   // for each var
  //   if (en->is_func == 0) {
  //     for (en2 = en->next; en2 != NULL; en2 = en2->next) {
  //       if (en2->is_func == 1) {
  //         // compare against all other funcs
  //         if (strcmp(en->name, en2->name) == 0)
  //           return 1; // not unique
  //       }
  //     }
  //   }
  // }
  // return 0;
}

int match_num_args_func(struct ast *node) {
   if (node->ntoken == 2) {
      //printf("%s is var\n",node->token);
      struct table_entry *en = st_find_entry(node->token,"prog");
      if (en == NULL) {
         printf("Function %s not declared\n",node->token);
         return 1;
      }
      int use_num_child = get_child_num(node);
      if (use_num_child != en->num_arg) {
         // printf("Number of args NOT match function %s declaration FAIL\n",node->token);
         printf("Error: Number of arguments in function %s does not match definition\n",node->token);
         return 1;
      }
      // printf("Number of args match function %s declaration SUCCESS\n",node->token);
   }
   return 0;
}

int init_map(struct ast *node) {
   int type = 2;

   // if this node is defined in symbol table, use that type
   // if this is a funcdef, search by nodeid
   struct table_entry *st_en = st_find_by_id(node->id);
   if (st_en != NULL) 
      type = st_en->type;
   
   // if this is the name of a func, search by name and scope="prog"
   st_en = st_find_entry(node->token, "prog");
   if (st_en != NULL)
      type = st_en->type;
   
   // if this node is a const/explicit, use that type
   else if (isArithematic(node->token) || isArithematicConst(node->token) \
      || !strcmp(node->token, "int") || !strcmp(node->token, "ret int")) 
      type = 1;
   else if (isBoolean(node->token) || isBooleanConst(node->token) \
      || !strcmp(node->token, "bool") || !strcmp(node->token, "ret bool"))
      type = 0; 

   tm_append(node, type);
   return 0;
}

int fill_map(struct ast *node) {
   struct map_entry *tm_cur, *tm_tmp;
   struct table_entry *st_tmp;
   struct ast *a_tmp;
   int i;
   // skip if already set
   tm_cur = tm_find(node);
   if (tm_cur->type == 2) {
      //printf("mapping %i:%s\n", node->id, node->token);
      
      if (!strcmp(node->token, "if")) {
         // if second child has type, take it
         tm_tmp = tm_find(node->child->next->id);
         if (tm_tmp->type != 2)
            tm_cur->type = tm_tmp->type;
         // if third child has type, take it
         tm_tmp = tm_find(node->child->next->next->id);
         if (tm_tmp->type != 2)
            tm_cur->type = tm_tmp->type;
      }
      // var declaration
      else if (!strcmp(node->parent->token, "funcdef")) {
         // find matching in args
         st_tmp = st_find_by_id(node->parent->id);
         for(i=0; i<st_tmp->num_arg; i++) {
            if (!strcmp(node->token, find_ast_node(st_tmp->args[i].id)->token)) {
               tm_cur->type = st_tmp->args[i].type;
               break;
            }
         }
      }
      // var use
      else {
         a_tmp = node->parent;
         // walk up until let def (if exists)
         while (a_tmp != NULL && strcmp(a_tmp->token, "let")) 
            a_tmp = a_tmp->parent;
         // in a let
         if (a_tmp != NULL) {
            // found let. compare args            
            st_tmp = st_find_by_id(a_tmp->id);
            for (i=0; i<st_tmp->num_arg; i++) {
               if (!strcmp(node->token, find_ast_node(st_tmp->args[i].id)->token)) {
                  tm_cur->type = st_tmp->args[i].type;
                  break;
               }
            }
         }
         // must be in a funcdef
         else {
            st_tmp = st_find_by_id(get_root(node)->id);
            for(i=0; i<st_tmp->num_arg; i++) {
               if (!strcmp(node->token, find_ast_node(st_tmp->args[i].id)->token)) {
                  tm_cur->type = st_tmp->args[i].type;
                  break;
               }
            }
         }
      }
   }
   // loop while there are unknowns to discover
   if (tm_contains_unknowns())
      return 0; // change to 0
   else
      return 1;
}

int well_formed_aop(struct ast *node) {
   if (node->ntoken == 4) {
      int n = get_child_num(node);
      int i = 0;
      struct ast_child *ptr = node->child;
      for (i = 0; i < n; i ++) {
         //printf("node %s\n",ptr->id->token);
         struct map_entry *en = tm_find(ptr->id);
         if (en == NULL) {
            return 1; // entry not in the map
         }
         else {
            if (en->type != 1) {
               printf("Error: arguments of %s do not type check\n",node->token);
               return 1;
            }
         }
         ptr = ptr->next;
      }
   }
   return 0;
}

int well_formed_bop(struct ast *node) {
   if (node->ntoken == 5) {
      int n = get_child_num(node);
      int i = 0;
      struct ast_child *ptr = node->child;
      for (i = 0; i < n; i ++) {
         //printf("node %s\n",ptr->id->token);
         struct map_entry *en = tm_find(ptr->id);
         if (en == NULL) {
            return 1; // entry not in the map
         }
         else {
            if (en->type != 0) {
               printf("Error: arguments of %s do not type check\n",node->token);
               return 1;
            }
         }
         ptr = ptr->next;
      }
   }
   return 0;
}
