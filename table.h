#ifndef TABLE_H
#define TABLE_H

struct arg {
    int id;
    int type;
};

struct table_entry {
    char name[50];
    int type; // 0 for int, 1 for bool
    int node_id;
    char scope[50];
    int let_id;
    int is_func;
    struct arg args[50];
    int num_arg;
    struct table_entry *next;
};
struct sym_table {
    struct table_entry *start;
};

void st_append(char *name, int type, int node_id, char *scope, int let_id, struct arg args[], int num_arg, int is_func);
int st_exists(char *name);
int st_exists_in(char *name, char *scope);
int st_get_type(struct ast *);
void st_print();
void st_free();
struct table_entry *st_find_entry(char *name, char *scope);
struct table_entry *get_entry(char *name, int id);
struct table_entry *get_func(char *name);
int is_func_unique(char *name);
struct table_entry *find_by_id(int node_id);
#endif
