#include "map.h"
#include <stdio.h>
#include "ast.h"

struct type_map map;

void tm_append(struct ast *node, int type) {
    struct map_entry *newm = (struct map_entry *) malloc(sizeof(struct map_entry));

    newm->node = node;
    newm->type = type;
    newm->next = NULL;

    if (map.start == NULL) {
        map.start = newm;
    }
    else {
        struct map_entry *en = map.start, *pr;
        while (en != NULL) {
            if (en->node == node)
                return; // already exists in list, skip.
            pr = en;
            en = en->next;
        }
        pr->next = newm;
    }
}

int tm_contains_unknowns() {
    struct map_entry *en = map.start;
    while (en != NULL) {
        if (en->type == 2) {
            //printf("%s %i is unknown\n", en->node->token, en->node->id);
            return 1;
        }
        en = en->next;
    }
    return 0;
}

void tm_print() {
    struct map_entry *en = map.start;
    printf("%10s %10s %10s\n", "token", "nodeid", "type");
    while (en != NULL) {
        printf("%10s %10i %10i\n", en->node->token, en->node->id, en->type);
        en = en->next;
    }
}

void tm_free() {
   struct map_entry *en;
   for (en = map.start; en != NULL;) {
      struct map_entry *temp = en->next;
      free(en);
      en = temp;
   }    
}

int tm_exists(struct ast *node) {
   struct map_entry *en;
   for (en = map.start; en != NULL; en=en->next) {
      if (en->node == node)
         return 1;
   }  
   return 0;
}

struct map_entry* tm_find(struct ast *node) {
   struct map_entry *en;
   for (en = map.start; en != NULL; en=en->next) {
      if (en->node == node)
         return en;
   }  
   return NULL;
}

struct map_entry* tm_find_by_name(char *name){
   struct map_entry *en;
   for (en = map.start; en != NULL; en=en->next) {
      if (!strcmp(en->node->token, name))
         return en;
   }  
   return NULL;
}