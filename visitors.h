#ifndef VISITORS_H
#define VISITORS_H
#include "ast.h"

void visit(struct ast *node);

int fill_table(struct ast *node);

int declare_var_before_use(struct ast *node);

int declare_func_before_use(struct ast *node);

int unique_func_names(struct ast *node);

int vars_with_func_names(struct ast *node);

int match_num_args_func(struct ast *node);

int duplicate_var_declare(struct ast *node);

int duplicate_arg_func(struct ast *node);
   
int init_map(struct ast *node);

int fill_map(struct ast *node);

int well_formed_aop(struct ast *node);

int well_formed_bop(struct ast *node);

int func_call_args_type(struct ast *node);

#endif
