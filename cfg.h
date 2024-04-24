#ifndef CFG_H
#define CFG_H
#define MAX_LINE_NUM 255
#define MAX_LINE_SIZE 255
#define MAX_REG_LEN 5
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "helpers.h"
#include "map.h"

struct reg_map {
    char *reg;
    struct ast *node;
    struct reg_map *next;
};

struct line {
    char *text;
    struct line *next;
};

struct bblk {
    int num;
    bool done;
    bool visited;
    struct ast *node;
    struct line *lines;
    struct bblk *up;
    struct bblk *down;
    struct bblk_child *child;
    struct bblk_parent *parent;
    struct bblk *next;
};

struct bblk_child {
   struct bblk *id;
   struct bblk_child *next;
};

struct bblk_parent {
   struct bblk *id;
   struct bblk_parent* next;
};

struct funcs {
    struct bblk *func;
    struct funcs *next;
    struct funcs *prev;
};

struct reg_map *rm_create(char *reg, struct ast *node);
struct reg_map *rm_get(struct ast *node);
void rm_add(struct reg_map *node);

struct line *create_line(char *text);
struct bblk *create_bblk(struct ast *node, struct line *line);
struct funcs *create_func(struct bblk *blk);
void insert_blk(struct bblk *node);
void add_function(struct funcs *func);
void append_line(struct bblk *blk, struct line *line);
void cfg_print();
void cfg_destroy();
int cfg(struct ast *node);
int cfg_construct(struct ast *node);
void cfg_dot();

void merge_blocks(int *changes);
void eliminate_unreachable_code(int *changes);
void duplicate_branch_elimination(int *changes);

#endif
