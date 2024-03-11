#ifndef VISITORS_H
#define VISITORS_H
#include "ast.h"
#include "table.h"

void visit(struct ast *node);

int fill_table(struct ast *node);

int declare_var_before_use(struct ast *node);

int declare_func_before_use(struct ast *node);

int unique_func_names(struct ast *node);

int vars_with_func_names(struct ast *node);

int match_num_args_func(struct ast *node);

int unique_vars_in_scope();

int duplicate_var_declare(struct ast *node);

#endif
