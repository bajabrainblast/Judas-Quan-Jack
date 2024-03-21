#include "helpers.h"
#include "table.h"

int isArithematic(char *op){
  return !strcmp(op, "+") || !strcmp(op, "-") || !strcmp(op, "*") ||
         !strcmp(op, "div") || !strcmp(op, "mod") || !strcmp(op, "get-int");
}

int isArithematicConst(char *op) {
  if (op[0] == '-') {

  }
  else
    return 0;
}

int isBoolean(char *op){
  return !strcmp(op, "=") || !strcmp(op, "<") || !strcmp(op, ">") ||
         !strcmp(op, "<=") || !strcmp(op, ">=") || !strcmp(op, "not") ||
         !strcmp(op, "and") || !strcmp(op, "or")|| !strcmp(op, "get-bool");
}

int isBooleanConst(char *op) {
  return !strcmp(op, "True") || !strcmp(op, "true") ||
         !strcmp(op, "False") || !strcmp(op, "false");
}

int getType(struct ast *node){
  if (isBoolean(node->token)) return 0;
  if (isArithematic(node->token)) return 1;
  if (!strcmp(node->token, "let")) return getType(node->child->next->id);
  if (!strcmp(node->token, "if")) return getType(node->child->next->id);
  return st_get_type(node);
}

int insertArg(struct arg args[], int id, int type){
  int i;
  for (i = 0; i < 50; i++){
    if (args[i].id == id && args[i].type == type) {
      return 0;
    }
    if (args[i].id == 0){
      args[i].id = id;
      args[i].type = type;
      return 1;
    }
  }
  return 0;
}

void findNestedLetVars(struct ast *node, char *token, int id){
  if (!strcmp(node->token, "let")){
    struct table_entry *l_en = st_get_entry(node->token, node->id), *v_en = st_get_entry(token, id);
    l_en->num_arg += insertArg(l_en->args, v_en->node_id, v_en->type);
  }
  for (struct ast_child *child = node->child; child; child = child->next){
    findNestedLetVars(child->id, token, id);
  }
}