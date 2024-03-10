#include "helpers.h"
#include "table.h"

int isArithematic(char *op){
  return !strcmp(op, "+") || !strcmp(op, "-") || !strcmp(op, "*") ||
         !strcmp(op, "div") || !strcmp(op, "mod") || !strcmp(op, "getint");
}

int isBoolean(char *op){
  return !strcmp(op, "=") || !strcmp(op, "<") || !strcmp(op, ">") ||
         !strcmp(op, "<=") || !strcmp(op, ">=") || !strcmp(op, "not") ||
         !strcmp(op, "and") || !strcmp(op, "or")|| !strcmp(op, "getbool");
}

int getType(struct ast *node){
  printf("%s\n", node->token);
  if (isBoolean(node->token)) return 0;
  if (isArithematic(node->token)) return 1;
  if (!strcmp(node->token, "let")) return getType(node->child->next->id);
  if (!strcmp(node->token, "if")) return getType(node->child->next->id);
  return st_get_type(node);
}

