#ifndef HELPERS_H
#define HELPERS_H
#include "ast.h"

int isArithematic(char *op);

int isBoolean(char *op);

int getType(struct ast *node);

#endif
