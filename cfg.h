#ifndef CFG_H
#define CFG_H
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
    struct ast *node;
    struct line *lines;
    struct bblk *up;
    struct bblk *down;
};

struct funcs {
    struct bblk *func;
    struct funcs *next;
};

struct reg_map *rm_create(char *reg, struct ast *node);
struct reg_map *rm_get(struct ast *node);
void rm_add(struct reg_map *node);

struct line *create_line(char *text);
struct bblk *create_bblk(struct ast *node, struct line *line);
struct funcs *create_func(struct bblk *blk);
void insert_blk(struct bblk *node);
void add_function(struct funcs *func);
void cfg_print();
void cfg_destroy();
int cfg(struct ast *node);
#endif