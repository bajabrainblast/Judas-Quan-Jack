#include "cfg.h"
struct bblk *top;
struct bblk *end;
struct bblk_child *bblk_child_root;
struct bblk_child *bblk_child_end;
struct funcs cfgs;
struct reg_map *rmap;
int counter = 1;

struct reg_map *rm_create(char *reg, struct ast *node){
    struct reg_map *mapping = (struct reg_map *) malloc(sizeof(struct reg_map));
    mapping->node = node;
    mapping->reg = reg;
    return mapping;
}

struct reg_map *rm_get(struct ast *node){
    if (!rmap) return NULL;
    struct reg_map *i;
    for (i = rmap; i; i = i->next) if (i->node == node) return i;
    return NULL;
}

void rm_add(struct reg_map *node){
    if (!rmap) rmap = node;
    struct reg_map *mapping;
    for (mapping = rmap; mapping->next; mapping = mapping->next);
    mapping->next = node;
}

struct line *create_line(char *text){
    struct line *line = (struct line *)malloc(sizeof(struct line));
    line->text = strdup(text);
    line->next = NULL;
    return line; 
}

struct bblk *create_bblk(struct ast *node, struct line *line){
    struct bblk *blk = (struct bblk *)malloc(sizeof(struct bblk));
    blk->num = 0;
    blk->done = false;
    blk->node = node;
    blk->lines = line;
    blk->up = NULL;
    blk->down = NULL;
    blk->bblk_child = NULL;
    blk->next = NULL;
    return blk;
}

struct funcs *create_func(struct bblk *blk){
    struct funcs *func = (struct funcs *)malloc(sizeof(struct funcs));
    func->func = blk;
    func->next = NULL;
    return func;
}

void insert_blk(struct bblk *node){
    if (!top) return;
    struct bblk *cblk;
    for (cblk = top; cblk->down; cblk = cblk->down);
    cblk->down = node;
    node->up = cblk;
    node->num = node->up->num + 1;
}

void add_function(struct funcs *func){
    if (!cfgs.func){
        cfgs.func = func->func;
        return;
    }
    struct funcs *cfunc;
    for (cfunc = &cfgs; cfunc->next; cfunc = cfunc->next);
    cfunc->next = func;
}

void cfg_print(){
    if (!cfgs.func){
        printf("No CFG for any function.\n");
        return;
    }
    struct funcs *func;
    for (func = &cfgs; func; func = func->next){
        printf("Func %s\n", func->func->node->token);
        struct bblk *blk;
        for (blk = func->func; blk; blk = blk->down){
            printf("Block %d:\n", blk->num);
            struct line *line;
            for (line = blk->lines; line; line = line->next) printf("\t%s\n", line->text);
        }
    }
}

void cfg_destroy() {
   struct bblk_child *cbblk_child = bblk_child_root;
   struct bblk *cblk = top;
   while (cbblk_child) {
      struct bblk_child *tbbk_child = cbblk_child;
      cbblk_child = cbblk_child->next;
      free(tbblk_child);
   }
   while (cblk) {
      struct bbk *tblk = cblk;
      cblk = cblk->next;
      free(tblk->lines->text);
      free(tblk);
   }
};

char *get_vreg(struct ast *node, char reg[]){
    sprintf(reg, "v%d", node->id);
}

char *form_text(struct ast *node, int type){
    char *text = (char *) malloc(MAX_LINE_SIZE);
    char reg[MAX_REG_LEN],src1[MAX_REG_LEN], src2[MAX_REG_LEN];
    get_vreg(node, reg);
    switch(type){
        case 0:
            sprintf(text, "%s := %s", reg, node->token);
            break;
        case 1:
            get_vreg(node->child->id, src1);
            get_vreg(node->child->next->id, src2);
            if (!strcmp(node->token, "-")) sprintf(text, "%s := %s MINUS %s", reg, src1, src2);
            if (!strcmp(node->token, "div")) sprintf(text, "%s := %s DIV %s", reg, src1, src2);
            if (!strcmp(node->token, "mod")) sprintf(text, "%s := %s MOD %s", reg, src1, src2);
            if (!strcmp(node->token, "+")){
                sprintf(text, "%s := %s PLUS %s", reg, src1, src2);
                struct ast_child *child;
                for (child = node->child->next->next; child; child = child->next){
                    get_vreg(child->id, src2);
                    strcat(text, strcat(" PLUS ", src2));
                }
            }
            if (!strcmp(node->token, "*")){
                sprintf(text, "%s := %s MULT %s", reg, src1, src2);
                struct ast_child *child;
                for (child = node->child->next->next; child; child = child->next){
                    get_vreg(child->id, src2);
                    strcat(text, strcat(" MULT ", src2));
                }
            }
            break;
        case 2:
            get_vreg(node->child->id, src1);
            if (!strcmp(node->token, "not")){
                sprintf(text, "NOT %s", src1);
                break;
            }
            get_vreg(node->child->next->id, src2);
            if (!strcmp(node->token, "=")) sprintf(text, "%s := %s EQ %s", reg, src1, src2);
            if (!strcmp(node->token, "<")) sprintf(text, "%s := %s LT %s", reg, src1, src2);
            if (!strcmp(node->token, ">")) sprintf(text, "%s := %s GT %s", reg, src1, src2);
            if (!strcmp(node->token, "<=")) sprintf(text, "%s := %s LEQ %s", reg, src1, src2);
            if (!strcmp(node->token, ">=")) sprintf(text, "%s := %s GEQ %s", reg, src1, src2);
            if (!strcmp(node->token, "and")){
                sprintf(text, "%s := %s AND %s", reg, src1, src2);
                struct ast_child *child;
                for (child = node->child->next->next; child; child = child->next){
                    get_vreg(child->id, src2);
                    strcat(text, strcat(" AND ", src2));
                }
            }
            if (!strcmp(node->token, "or")){
                sprintf(text, "%s := %s OR %s", reg, src1, src2);
                struct ast_child *child;
                for (child = node->child->next->next; child; child = child->next){
                    get_vreg(child->id, src2);
                    strcat(text, strcat(" OR ", src2));
                }
            }
            break;
        case 3:
            sprintf(text, "%s := %s(", reg, node->token);
            struct ast_child *child;
            for (child = node->child; child; child = child->next){
                if (child != node->child) strcat(text, ", ");
                get_vreg(child->id, src1);
                strcat(text, src1);
            }
            strcat(text, ")");
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        default:
            printf("How did you get here?\n");
    }
    return text;
}

bool cfg_all_done() {
   struct bblk *cblk;
   cblk = top;
   while (cblk) {
      if (!cblk->done) {
         return false;
      }
      cblk = cblk->down;
   }
   return true;
}

void insert_bblk_up(struct bblk *cblk, struct bblk *tblk) {
   cblk->up->down = tblk;
   tblk->up = cblk->up;
   tblk->down = cblk;
   cblk->up = tblk;
   end->next = tblk;
   end = tblk;
}

void insert_child(struct bblk *cblk, struct bblk *tblk) {
   struct bblk_child *child = (struct bblk_child *) malloc(sizeof(struct bblk_child));
   child->id = tblk;
   if (cblk->child == NULL) {
      cblk->child = child;
   }
   else {
      struct bblk_child *cbblk_child;
      for (cbblk_child = cblk->child; cbblk_child->next; cbblk_child = cbblk->next){};
      cbblk_child->next = child;
   }
   if (bblk_child_root == NULL) {
      bblk_child_root = child;
      bblk_child_end = child;
   }
   else {
      bblk_child_end->next = child;
      bblk_child_end = child;
   }
}

int cfg_construct(struct ast *node) {
   if (!strcmp(node->token, "funcdef")) {
      struct line *text = create_line(node->child->id->token);
      char var[10];
      top = create_blk(node->child->id, node->child->id->token);
      top->done = true;
      sprintf(var,"v%d",counter);
      counter ++;
      struct ast_child *lchild;
      struct bblk *func_main;
      for (lchild = node->child; lchild->next; lchild=lchild->next);
      func_main = create_blk(lchild->id,var);
      func_main->up = top;
      top->next = func_main;
      end = func_main;

      while(!cfg_all_done()) {
         struct bblk *cblk = top;
         while (cblk) {
            if (!cblk->done) {
               struct ast *cnode = cblk->node;
               char val[36];
               if (cnode->child == NULL) {
                  sprintf(val,"%s := %s",cblk->lines->text, cnode->token);
               }
               else if (isArithematic(cnode->token) || isBoolean(cnode->token)) {
                  struct ast_child *cchild = cnode->child;
                  char opt[6];
                  sprintf(opt," %s ", cnode->token);
                  strcpy(val,cblk->lines->text);
                  strcat(val," := ")
                  for(;cchild;cchild=cchild->next) {
                     struct bblk *blk_tmp;
                     char var_tmp[10];
                     sprintf(var_tmp,"v%d",counter);
                     counter ++;
                     blk_tmp = create_blk(cchild->id, var_tmp);
                     insert_bblk_up(cblk,blk_tmp);
                     strcat(val,var_tmp);
                     if (cchild->next != NULL)
                        strcat(val,opt);
                  }
               }
               else if (!strcmp(cnode->token,"let")) {
                  struct bblk *v1 = NULL;
                  struct bblk *v2 = NULL;
                  struct bblk *v3 = NULL;
                  char var_tmp[10];
                  char var_def[36];
                  sprintf(var_tmp,"v%d",counter);
                  counter ++;
                  v2 = create_blk(cnode->child->next->id, var_tmp);
                  insert_bblk_up(cblk,v2);
                  sprintf(var_def,"%s := %s",cnode->child->id->token,var_tmp);
                  v1 = create_blk(cnode->child->id, var_def);
                  insert_bblk_up(cblk,v1);
                  sprintf(var_tmp,"v%d",counter);
                  counter ++;
                  v3 = create_blk(cnode->child->next->next->id, var_tmp);
                  insert_bblk_up(cblk,v3);
                  sprintf(val,"%s := %s",cblk->lines->text,var_tmp);
               }
               else if (!strcmp(cnode->token,"if")) {
               }
               free(cnode->lines->text);
               cnode->linex->text = strdup(val);
               cblk->done = true;
            }
            cblk = cblk->next;
         }
      }
   }
}

int cfg(struct ast *node){
  char reg[MAX_REG_LEN];
  if (!node->parent) return 0;
  struct ast_child *fchild = node->parent->child, *lchild;
  for (lchild = node->parent->child; lchild->next; lchild = lchild->next){}
  if (!strcmp(node->parent->token, "funcdef") && node != fchild->id && node != lchild->id){
    return 0;
  }
  if (st_find_by_id(node->id) && !strcmp(st_find_by_id(node->id)->scope, "PEP") && strcmp(top->lines->text, "main")){
    add_function(create_func(top));
    struct line *text = create_line("main");
    top = create_bblk(tm_find_by_name("PEP")->node, text);
    top->num = 1;
    add_function(create_func(top));
  }
  if (strcmp(node->parent->token, "PEP")){
    if (node == fchild->id && !strcmp(node->parent->token, "funcdef")){
      printf("Function: %s\n", node->token);
      // Hook up previous function cfg to global tree if exists
      if (top){
        add_function(create_func(top));
      }
      // Start new Cfg for functions
      struct line *text = create_line(node->token);
      top = create_bblk(node, text);
      top->num = 1;
    } else if (isArithematic(node->token)){
      printf("Arithematic Node: %s\n", node->token);
      struct line *text = create_line(form_text(node, 1));
      insert_blk(create_bblk(node, text));
      rm_create(get_vreg(node, reg), node);
    } else if (isBoolean(node->token)){
      printf("Boolean Node: %s\n", node->token);
      struct line *text = create_line(form_text(node, 2));
      insert_blk(create_bblk(node, text));
      rm_create(get_vreg(node, reg), node);
    } else if (isArithematicConst(node->token)){
      printf("Arithematic Const Node: %s\n", node->token);
      // Insert block
      struct line *text = create_line(form_text(node, 0));
      insert_blk(create_bblk(node, text));
      rm_create(get_vreg(node, reg), node);
    } else if (isBooleanConst(node->token)){
      printf("Boolean Const Node: %s\n", node->token);
      // Insert block
      struct line *text = create_line(form_text(node, 0));
      insert_blk(create_bblk(node, text));
      get_vreg(node, reg);
      rm_create(reg, node);
    } else {
      if (st_get_func(node->token)){
        printf("Function Call Node: %s\n", node->token);
        struct line *text = create_line(form_text(node, 3));
        insert_blk(create_bblk(node, text));
        get_vreg(node, reg);
        rm_create(reg, node);
      } else if (!strcmp(node->token, "if")){
        printf("If Node: %s\n", node->token);  
      } else if (!strcmp(node->token, "let")){
        printf("Let Node: %s\n", node->token);  
      } else {
        printf("Variable Node: %s\n", node->token);
        struct line *text = create_line(form_text(node, 0));
        insert_blk(create_bblk(node, text));
        get_vreg(node, reg);
        rm_create(reg, node);
      }
    }
  }

  return 0;
}

void cfg_dot() {
    FILE *fp = fopen("cfg.dot", "w");
    fprintf(fp, "digraph print {\n ");
    
    struct funcs *func;
    for (func=&cfgs; func; func=func->next) {
        struct bblk *blk, *pblk = func->func;
        fprintf(fp, "%d [label=\"%i: %s\", fontname=\"monospace\", style=filled, fillcolor=mintcream];\n ", func->func->node->id, func->func->node->id, func->func->lines->text);
        // skip the first blk as its just func name and is handled above
        for (blk=func->func->down; blk; blk=blk->down) {
            // not sure about the lines? when will there be more than just one?
            struct line *line;
            for (line=blk->lines; line; line=line->next) {
                fprintf(fp, "%d [label=\"%i: %s\", fontname=\"monospace\"];\n ", blk->node->id, blk->node->id, line->text);
            }
            
            fprintf(fp, "%d->%d\n ", pblk->node->id, blk->node->id);

            // if the prev block is an if, do not update pblk
            if (strncmp(pblk->node->token, "if", 2))
                pblk = blk;
        }
    }
    
    fprintf(fp, "}\n ");
    fclose(fp);
    system("dot -Tpdf cfg.dot -o cfg.pdf");
    return;
}
