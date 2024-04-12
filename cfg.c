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
    blk->parent = NULL;
    blk->child = NULL;
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
   struct bblk_child *cchild;
   struct bblk_parent *cparent;
   struct funcs *func;
   for (func=&cfgs; func; func=func->next) {
      if (func->func) {
         struct bblk *cblk = func->func;
         while (cblk) {
            struct bblk *tblk = cblk;
            cblk = cblk->next;
            cparent = tblk->parent;
            while (cparent) {
               struct bblk_parent *tparent = cparent;
               cparent = cparent->next;
               free(tparent);
            }
            cchild = tblk->child;
            while (cchild) {
               struct bblk_child *tchild = cchild;
               cchild = cchild->next;
               free(cchild);
            }
            free(tblk->lines->text);
            free(tblk);
         }
      }
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
      cblk = cblk->next;
   }
   return true;
}

void add_child(struct bblk *cblk, struct bblk *tblk) {
   struct bblk_child *child = (struct bblk_child *) malloc(sizeof(struct bblk_child));
   child->id = tblk;
   if (cblk->child == NULL) {
      cblk->child = child;
   }
   else {
      struct bblk_child *cbblk_child;
      for (cbblk_child = cblk->child; cbblk_child->next; cbblk_child = cbblk_child->next){};
      cbblk_child->next = child;
   }
}

void remove_parent(struct bblk *cblk, struct bblk *tblk) {
   struct bblk_parent *cparent = cblk->parent;
   if (cparent->id == tblk) {
      cblk->parent = cparent->next;
      free(cparent);
   }
   else {
      while (cparent->next) {
         if (cparent->next->id == tblk) {
            struct bblk_parent *tparent = cparent->next;
            cparent->next = cparent->next->next;
            free(tparent);
         }
      }
   }
}

void populate_child() {
   struct bblk *cblk = top;
   while (cblk) {
      struct bblk_parent *cparent = cblk->parent;
      while (cparent) {
         add_child(cparent->id,cblk);
         cparent = cparent->next;
      }
      cblk = cblk->next;
   }
}

void add_parent(struct bblk *cblk, struct bblk *tblk) {
   struct bblk_parent *cparent = cblk->parent;
   struct bblk_parent *tparent = (struct bblk_parent *) malloc(sizeof(struct bblk_parent));
   tparent->id = tblk;
   if (cparent == NULL) {
      cblk->parent = tparent;
   }
   else {
      while(cparent->next) {
         cparent = cparent->next;
      }
      cparent->next = tparent;
   }
}

void insert_bblk_up(struct bblk *cblk, struct bblk *tblk) {
   struct bblk_parent *cparent = cblk->parent;
   if (cparent == NULL) {
      add_parent(cblk,tblk);
   }
   else {
      while (cparent) {
         struct bblk *parent_node = cparent->id;
         add_parent(tblk,parent_node);
         cparent = cparent->next;
         remove_parent(cblk,parent_node);
      }
   }
   add_parent(cblk,tblk);
   end->next = tblk;
   end = tblk;
}

int cfg_construct(struct ast *node) {
   if (!strcmp(node->token, "funcdef") ||
       !strcmp(node->token, "PEP")) {
      struct line *text = create_line(node->child->id->token);
      char var[10];
      struct bblk *func_main;
      if (!strcmp(node->token, "funcdef")) {
         top = create_bblk(node->child->id, create_line(node->child->id->token));
         top->done = true;
         sprintf(var,"v%d",counter);
         counter ++;
         struct ast_child *lchild;
         for (lchild = node->child; lchild->next; lchild=lchild->next);
         func_main = create_bblk(lchild->id, create_line(var));
      }
      if (!strcmp(node->token, "PEP")) {
         top = create_bblk(node, create_line("PEP"));
         top->done = true;
         sprintf(var,"v%d",counter);
         counter ++;
         func_main = create_bblk(node->child->id, create_line(var));
      }
      add_parent(func_main,top);
      top->next = func_main;
      end = func_main;

      while(!cfg_all_done()) {
         struct bblk *cblk = top;
         while (cblk) {
            if (!cblk->done) {
               struct ast *cnode = cblk->node;
               char val[60];
               if (cnode->child == NULL) {
                  sprintf(val,"%s := %s",cblk->lines->text, cnode->token);
               }
               else if (isArithematic(cnode->token) || isBoolean(cnode->token)) {
                  struct ast_child *cchild = cnode->child;
                  char opt[6];
                  sprintf(opt," %s ", cnode->token);
                  strcpy(val,cblk->lines->text);
                  strcat(val," := ");
                  for(;cchild;cchild=cchild->next) {
                     struct bblk *blk_tmp;
                     char var_tmp[10];
                     sprintf(var_tmp,"v%d",counter);
                     counter ++;
                     blk_tmp = create_bblk(cchild->id, create_line(var_tmp));
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
                  v2 = create_bblk(cnode->child->next->id, create_line(var_tmp));
                  insert_bblk_up(cblk,v2);
                  sprintf(var_def,"%s := %s",cnode->child->id->token,var_tmp);
                  v1 = create_bblk(cnode->child->id, create_line(var_def));
                  v1->done = true;
                  insert_bblk_up(cblk,v1);
                  sprintf(var_tmp,"v%d",counter);
                  counter ++;
                  v3 = create_bblk(cnode->child->next->next->id, create_line(var_tmp));
                  insert_bblk_up(cblk,v3);
                  sprintf(val,"%s := %s",cblk->lines->text,var_tmp);
               }
               else if (!strcmp(cnode->token,"if")) {
                  struct bblk *v1 = NULL;
                  struct bblk *v2 = NULL;
                  struct bblk *v3 = NULL;
                  char var_tmp[10];
                  sprintf(var_tmp,"v%d",counter);
                  counter ++;
                  v1 = create_bblk(cnode->child->id, create_line(var_tmp));
                  insert_bblk_up(cblk,v1);
                  sprintf(var_tmp,"v%d",counter);
                  counter ++;
                  v2 = create_bblk(cnode->child->next->id, create_line(var_tmp));
                  sprintf(var_tmp,"v%d",counter);
                  counter ++;
                  v3 = create_bblk(cnode->child->next->next->id, create_line(var_tmp));

                  //manually link the blocks
                  /*
                  struct bblk_parent *cparent = cblk->parent;
                  while (cparent) {
                     struct bblk *parent_node = cparent->id;
                     add_parent(v1,parent_node);
                     cparent = cparent->next;
                     remove_parent(cblk,parent_node);
                  }
                  */
                  add_parent(v2,v1);
                  add_parent(v3,v1);
                  add_parent(cblk,v2);
                  add_parent(cblk,v3);
                  end->next = v2;
                  end = v2;
                  end->next = v3;
                  end = v3;
                  sprintf(val,"IF %s = true, then %s := %s, else %s := %s",v1->lines->text,cblk->lines->text,v2->lines->text,cblk->lines->text,v3->lines->text);
               }
               free(cblk->lines->text);
               cblk->lines->text = strdup(val);
               cblk->done = true;
            }
            cblk = cblk->next;
         }
      }
      populate_child();
      add_function(create_func(top));
   }
   return 0;
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

void add_nodes(FILE *fp, struct bblk* cblk) {
   struct bblk_child *cchild; 
   fprintf(fp, "%d [label=\"%i: %s\", fontname=\"monospace\", style=filled, fillcolor=mintcream];\n ", cblk->node->id, cblk->node->id, cblk->lines->text);
   for (cchild = cblk->child; cchild; cchild = cchild->next) {
      fprintf(fp, "%d->%d\n ", cblk->node->id, cchild->id->node->id);
      add_nodes(fp, cchild->id);
   }
}

void cfg_dot() {
    struct bblk *blk;
    struct bblk_child *chblk;
    struct funcs *func;
    struct line *line;
    FILE *fp;

    /*
    possible issues:
    when theres more than one line in the func head
    */

    fp = fopen("cfg.dot", "w");
    fprintf(fp, "digraph print {\n ");
    
    // add all nodes and links to dot file
    for (func=&cfgs; func; func=func->next) {
       if (func->func)
          add_nodes(fp,func->func);
    }
    fprintf(fp, "}\n ");
    fclose(fp);
    system("dot -Tpdf cfg.dot -o cfg.pdf");
    return;
}
