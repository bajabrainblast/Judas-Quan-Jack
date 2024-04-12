#ifndef MAP_H
#define MAP_H

struct map_entry {
    struct ast *node;
    int type; // 0 for bool, 1 for int, 2 for unknown
    struct map_entry *next;
};
struct type_map {
    struct map_entry *start;
};

void tm_append(struct ast *node, int type);
int tm_contains_unknowns(); // returns 1 if any unknown mappings, 0 otherwise
void tm_print();
void tm_free();
int tm_exists(struct ast *node);
struct map_entry* tm_find(struct ast *node);
struct map_entry* tm_find_by_name(char *name);

#endif