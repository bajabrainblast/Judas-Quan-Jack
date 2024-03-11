#include "ast.h"
#include "table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct sym_table table;

int let_count = 0;

void st_append(char *name, int type, int node_id, char *scope, int let_id, struct arg args[], int num_arg, int is_func) {
    struct table_entry *newt = (struct table_entry *) malloc(sizeof(struct table_entry));
    strcpy(newt->name, name);
    newt->type = type;
    strcpy(newt->scope, scope);
    newt->node_id = node_id;
    newt->let_id = let_id;
    newt->is_func = is_func;
    newt->next = NULL;
    int i;
    for (i = 0; i < num_arg; i ++) {
       newt->args[i].id = args[i].id;
       newt->args[i].type = args[i].type;
    }
    newt->num_arg = num_arg;

    if (table.start == NULL) {
        table.start = newt;
    }
    else {
        struct table_entry *en = table.start, *pr;
        while (en != NULL) {
            pr = en;
            en = en->next;
        }
        pr->next = newt;
    }
}

int st_exists(char *name) {
    struct table_entry *en;
    for (en = table.start; en != NULL; en = en->next) {
        if (strcmp(name, en->name) == 0)
            return 1;
    }
    return 0;
}

int st_exists_in(char *name, char *scope) {
    struct table_entry *en;
    for (en = table.start; en != NULL; en = en->next) {
        if (strcmp(name, en->name) == 0) {
            if (strcmp(scope, en->scope) == 0 || strcmp("prog", en->scope) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

int st_get_type(struct ast *node){
   if (!st_exists(node->token)) return -1;
   struct table_entry *en;
   for (en = table.start; en; en = en->next) {
   if (!strcmp(node->token, en->name))
      return en->type;
   }
   return (-1);
}

void st_print() {
    struct table_entry *en;
    printf("%10s %10s %10s %10s %10s %35s\n", "name", "type", "scope", "let_id", "is_func","args");
    for (en = table.start; en != NULL; en = en->next) {
        char args_txt[30] = "";
        int i;
        for (i = 0; i < en->num_arg; i ++) {
           if (en->args[i].id == 0) continue;
           char arg_item[14];
           if (i == 0)
              sprintf(arg_item,"%s :",find_ast_node(en->args[i].id)->token);
           else
              sprintf(arg_item,", %s :",find_ast_node(en->args[i].id)->token);
           if (en->args[i].type == 0)
              strcat(arg_item," bool");
           if (en->args[i].type == 1)
              strcat(arg_item," int");
           strcat(args_txt,arg_item);
        }
        if (!en) continue;
        printf("%10s %10i %10s %10i %10i %35s\n", en->name, en->type, en->scope, en->let_id, en->is_func,args_txt);
    }
}

void st_free() {
   struct table_entry *en;
   for (en = table.start; en != NULL;) {
      struct table_entry *temp = en->next;
      free(en);
      en = temp;
   }
}

struct table_entry* st_find_entry(char *name, char *scope) {
   struct table_entry *en;
   for (en = table.start; en != NULL; en = en->next) {
      if (strcmp(en->name,name) == 0 &&  strcmp(en->scope,scope) == 0) {
         return en;
      }
   }
   return NULL;
}

struct table_entry *get_entry(char *name, int id){
    struct table_entry *en;
    for (en = table.start; en != NULL; en = en->next) {
        if (!strcmp(en->name,name) &&  id == en->node_id) {
            return en;
        }
    }
   return NULL;
}

struct table_entry *get_func(char *name){
    struct table_entry *en;
    for (en = table.start; en != NULL; en = en->next) {
        if (en->is_func && !strcmp(en->name,name)) {
            return en;
        }
    }
   return NULL;
}

int is_func_unique(char *name){
    int count = 0;
    struct table_entry *en;
    for (en = table.start; en != NULL; en = en->next) {
        if (en->is_func && !strcmp(en->name,name)) {
            count++;
        }
    }
   return (count > 1);
}