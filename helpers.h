#ifndef HELPERS_H
#define HELPERS_H
#include "ast.h"
#include "table.h"

int isArithematic(char *op);

int isBoolean(char *op);

int getType(struct ast *node);

int insertArg(struct arg [], int id, int type);

void findNestedLetVars(struct ast *node, char *token, int type);

#endif
