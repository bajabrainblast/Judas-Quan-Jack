#include "table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct sym_table table;

int let_count = 0;

void st_append(char *name, int type, int node_id, char *scope, int let_id, int is_func) {
    struct table_entry *newt = (struct table_entry *) malloc(sizeof(struct table_entry));
    strcpy(newt->name, name);
    newt->type = type;
    strcpy(newt->scope, scope);
    newt->let_id = let_id;
    newt->is_func = is_func;
    newt->next = NULL;

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

void st_print() {
    struct table_entry *en;
    printf("%10s %10s %10s %10s %10s\n", "name", "type", "scope", "let_id", "is_func");
    for (en = table.start; en != NULL; en = en->next) {
        printf("%10s %10i %10s %10i %10i\n", en->name, en->type, en->scope, en->let_id, en->is_func);
    }
}
