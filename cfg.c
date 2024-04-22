#include "cfg.h"
struct bblk *top;
struct bblk *end;
struct funcs cfgs;
struct funcs *cfunc;
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
    blk->visited = false;
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

void append_line(struct bblk *blk, struct line *line){
   struct line *l;
   struct line *new_line = create_line(line->text);
   for (l = blk->lines; l->next; l = l->next);
   l->next = new_line;
}

void remove_line(struct bblk *blk, struct line *line){
   if (line == blk->lines){
      blk->lines = line->next;
   }
   // if (line)
};


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

void free_bblk(struct bblk *tblk) {
   struct line *cline;
   struct bblk_parent *cparent;
   struct bblk_child *cchild;
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
      free(tchild);
   }
   for (cline = tblk->lines; cline; cline = cline->next) {
      free(cline->text);
   }
   free(tblk);
}

void remove_bblk(struct funcs *func, struct bblk *rblk) {
   struct bblk *cblk;
   for (cblk = func->func; cblk->next != rblk; cblk = cblk->next) {
   }
   cblk->next = cblk->next->next;
   free_bblk(rblk);
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
            free_bblk(tblk);
         }
      }
   }
}

char *get_vreg(struct ast *node, char reg[]){
    sprintf(reg, "v%d", node->id);
    return reg;
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
   child->next = NULL;
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

void remove_child(struct bblk *cblk, struct bblk *tblk) {
   struct bblk_child *cchild = cblk->child;
   if (cchild->id == tblk) {
      cblk->child = cchild->next;
      free(cchild);
   }
   else {
      while (cchild->next) {
         if (cchild->next->id == tblk) {
            struct bblk_child *tchild = cchild->next;
            cchild->next = cchild->next->next;
            free(tchild);
         }
      }
   }
}

void populate_child() {
   struct bblk *cblk = top;
   while (cblk) {
      struct bblk_parent *cparent = cblk->parent;
      while (cparent) {
         struct bblk_parent *tparent = cparent;
         cparent = cparent->next;
         add_child(tparent->id,cblk);
      }
      cblk = cblk->next;
   }
}

void add_parent(struct bblk *cblk, struct bblk *tblk) {
   struct bblk_parent *cparent = cblk->parent;
   struct bblk_parent *tparent = (struct bblk_parent *) malloc(sizeof(struct bblk_parent));
   tparent->id = tblk;
   tparent->next = NULL;
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
   end->next = NULL;
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
         struct ast_child *lchild;
         for (lchild = node->child; lchild->next; lchild=lchild->next);
         get_vreg(lchild->id,var);
         func_main = create_bblk(lchild->id, create_line(var));
      }
      if (!strcmp(node->token, "PEP")) {
         top = create_bblk(node, create_line("PEP"));
         top->done = true;
         get_vreg(node->child->id,var);
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
                  if (st_get_func(cnode->token) != NULL) {
                     sprintf(val,"%s := %s()",cblk->lines->text, cnode->token);

                  }
                  else {
                     sprintf(val,"%s := %s",cblk->lines->text, cnode->token);
                  }
               }
               else if (isArithematic(cnode->token) || isBoolean(cnode->token)) {
                  struct ast_child *cchild = cnode->child;
                  char opt[6];
                  sprintf(opt," %s ", cnode->token);
                  strcpy(val,cblk->lines->text);
                  strcat(val," := ");
                  if (!strcmp(opt," not ")) {
                     strcat(val,"not ");
                  }
                  for(;cchild;cchild=cchild->next) {
                     struct bblk *blk_tmp;
                     char var_tmp[10];
                     get_vreg(cchild->id,var_tmp);
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
                  get_vreg(cnode->child->next->id,var_tmp);
                  v2 = create_bblk(cnode->child->next->id, create_line(var_tmp));
                  insert_bblk_up(cblk,v2);
                  sprintf(var_def,"%s := %s",cnode->child->id->token,var_tmp);
                  v1 = create_bblk(cnode->child->id, create_line(var_def));
                  v1->done = true;
                  insert_bblk_up(cblk,v1);
                  get_vreg(cnode->child->next->next->id,var_tmp);
                  v3 = create_bblk(cnode->child->next->next->id, create_line(var_tmp));
                  insert_bblk_up(cblk,v3);
                  sprintf(val,"%s := %s",cblk->lines->text,var_tmp);
               }
               else if (!strcmp(cnode->token,"if")) {
                  struct bblk *v1 = NULL;
                  struct bblk *v2 = NULL;
                  struct bblk *v3 = NULL;
                  char var_tmp[10];
                  get_vreg(cnode->child->id,var_tmp);
                  v1 = create_bblk(cnode->child->id, create_line(var_tmp));
                  insert_bblk_up(cblk,v1);
                  get_vreg(cnode->child->next->id,var_tmp);
                  v2 = create_bblk(cnode->child->next->id, create_line(var_tmp));
                  get_vreg(cnode->child->next->next->id,var_tmp);
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
                  remove_parent(cblk,v1);
                  end->next = v2;
                  end = v2;
                  end->next = v3;
                  end = v3;
                  sprintf(val,"IF %s = true, then %s := %s, else %s := %s",v1->lines->text,cblk->lines->text,v2->lines->text,cblk->lines->text,v3->lines->text);
               }
               // function call
               else {
                  struct ast_child *cchild = cnode->child;
                  strcpy(val,cblk->lines->text);
                  strcat(val," := ");
                  strcat(val,cnode->token);
                  strcat(val,"(");
                  for(;cchild;cchild=cchild->next) {
                     struct bblk *blk_tmp;
                     char var_tmp[10];
                     get_vreg(cchild->id,var_tmp);
                     blk_tmp = create_bblk(cchild->id, create_line(var_tmp));
                     insert_bblk_up(cblk,blk_tmp);
                     strcat(val,var_tmp);
                     if (cchild->next != NULL)
                        strcat(val,",");
                  }
                  strcat(val,")");
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
   if (cblk->visited) {
      return;
   }
   struct bblk_child *cchild; 
   cblk->visited = true;
   char text[MAX_LINE_SIZE * MAX_LINE_NUM] = "";
   strcat(text, cblk->lines->text);
   struct line *line;
   for (line = cblk->lines->next; line; line = line->next){
      strcat(text, "\n");
      strcat(text, line->text);
   }
   fprintf(fp, "%d [label=\"%i: %s\", fontname=\"monospace\", style=filled, fillcolor=mintcream];\n ", cblk->node->id, cblk->node->id, text);
   for (cchild = cblk->child; cchild; cchild = cchild->next) {
      fprintf(fp, "%d->%d\n ", cblk->node->id, cchild->id->node->id);
      add_nodes(fp, cchild->id);
   }
}

void reset_nodes(struct bblk* cblk) {
   /*
   struct bblk_child *cchild; 
   cblk->visited = false;
   for (cchild = cblk->child; cchild; cchild = cchild->next) {
      reset_nodes(cchild->id);
   }
   */
   struct bblk *tblk;
   for (tblk = cblk; tblk; tblk = tblk->next) {
      tblk->visited = false;
   }
}

void cfg_dot(char *name) {
    struct funcs *func;
    struct bblk *blk;
    struct bblk_child *cchild; 
    FILE *fp;
    char string[50];

    /*
    possible issues:
    when theres more than one line in the func head
    */
    sprintf(string, "%s.dot", name);
    fp = fopen(string, "w");
    fprintf(fp, "digraph print {\n ");
    
    // add all nodes and links to dot file
    for (func=&cfgs; func; func=func->next) {
       if (func->func)
          add_nodes(fp,func->func);
    }
    fprintf(fp, "}\n ");
    fclose(fp);
    sprintf(string, "dot -Tpdf %s.dot -o %s.pdf", name, name);
    system(string);

    // set all visited to 0 for possible second print
    for (func=&cfgs; func; func=func->next) {
       if (func->func)
          reset_nodes(func->func);
    }

    return;
}

void merge(struct bblk *parent, struct bblk *child){
   struct bblk_child *cchild;
   struct line *line;
   //parent->child = child->child;
   //parent->next = child->next;
   for (cchild = child->child; cchild; cchild = cchild->next){ 
      remove_parent(cchild->id, child);
      add_parent(cchild->id, parent);
      add_child(parent,cchild->id);
   }
   for (line = child->lines; line; line = line->next) append_line(parent, line);
   remove_child(parent,child);
   remove_bblk(cfunc,child);
   /*
   child->child = NULL;
   child->parent = NULL;
   child->lines = NULL;
   child->next = NULL;
   child->node = NULL;
   free(child);
   */
}

void merge_block(struct bblk *blk, int *changes){
   struct line *l;
   // for (l = blk->lines; l; l = l->next){
   //    printf("%s\n", l->text);
   // }
   struct bblk_child *child = blk->child;
   if (blk->child && !blk->child->next && !child->id->parent->next){
      // printf("Yay!\n");
      merge(blk, blk->child->id);
      (*changes)++;
      merge_block(blk, changes);
   }
   for (child = blk->child; child; child = child->next){
      merge_block(child->id, changes);
   }
}

void merge_blocks(int *changes){
   struct funcs *func;
   struct bblk *blk;
   struct line *l;
   for (func = &cfgs; func; func = func->next){
      cfunc = func;
      merge_block(func->func->child->id, changes);
      // printf("\n");
   }
}

// remove [start, finish)
void remove_bblk_between(struct bblk *start, struct bblk *finish, struct bblk *prev) {
   struct bblk_child *ch, *ch2, *cchild;
   struct bblk_parent *pr, *cparent;
   if (start == finish)
      return;
   for (ch=start->child; ch; ch=ch->next) 
      remove_bblk_between(ch->id, finish, start);
   printf("\tremoving %s\n", start->lines->text);
   // so what i really want is to remove the link from the current block to its children. 
   for (cparent = start->parent; cparent; cparent = cparent->next) {
      remove_child(cparent->id,start);
   }
   for (cchild = start->child; cchild; cchild = cchild->next) {
      remove_parent(cchild->id,start);
   }
   remove_bblk(cfunc,start);
   ch = start->child;
   //start->child = NULL;
}

// type = 0 means ifcond was set to false
void actual_elim(struct bblk *blk, struct line *if_line, struct bblk *gparent, int type) {
   // if ifcond was set to true, replace if_line text with then statement
   // ifcond was set to false, replace if_line text with else statement
   int i;
   struct bblk *rm_branch;
   char newstr[50] = "", *tmp, *tok;
   tmp = malloc(strlen(if_line->text) + 1);
   strcpy(tmp, if_line->text);
   tok = strtok(tmp, " ");

   // get the then
   if (type) {
      while(strcmp(tok, "then") != 0)
         tok = strtok(NULL, " ");
      // when tok = then, grab everything until the else
      tok = strtok(NULL, " ");
      while(strcmp(tok, "else") != 0) {
         strcat(newstr, tok);
         strcat(newstr, " ");
         tok = strtok(NULL, " ");
      }
      // remove the comma
      for (i=49; i>=0; i--) 
         if (newstr[i] == ',') {
            newstr[i] = '\0';
            break;
         }
      //printf("%s\n", newstr);
      rm_branch = gparent->child->next->id;
   }
   // get the else
   else {
      while(strcmp(tok, "else") != 0)
         tok = strtok(NULL, " ");
      tok = strtok(NULL, " ");
      while(tok) {
         strcat(newstr, tok);
         strcat(newstr, " ");
         tok = strtok(NULL, " ");
      }
      //printf("%s\n", newstr);
      rm_branch = gparent->child->id;
   }

   // remove all blocks between rm_branch and blk
   printf("removing all between %s\n\t and %s\n", rm_branch->lines->text, if_line->text);
   remove_bblk_between(rm_branch, blk, gparent);
   /*
   if (type) {
      gparent->child = NULL;
   }
   else {
      gparent->child = gparent->child->next;
      gparent->child->next = NULL;
   }
   */

   // replace old if with new str
   strcpy(if_line->text, newstr);

   free(tmp);
}

void find_cond_consts(struct bblk *blk, struct line *if_line, struct bblk *gparent, int *changes) {
   struct line *l;
   char ifcond[10], var[10], *tmp, *tok;

   // grab the if condition
   tmp = malloc(strlen(if_line->text) + 1);
   strcpy(tmp, if_line->text);
   tok = strtok(tmp, " ");
   tok = strtok(NULL, " ");
   strncpy(ifcond, tok, 10);
   free(tmp);

   // for each line in the grandparent, grab the first token (the register)
   // and test if its the same as ifcond
   for (l=gparent->lines; l; l=l->next) {
      tmp = malloc(strlen(l->text) + 1);
      strcpy(tmp, l->text);
      tok = strtok(tmp, " ");
      strncpy(var, tok, 10);

      // if they're the same reg, then test to see if token after next is "true" or "false"
      if (!strcmp(ifcond, tok)) {
         tok = strtok(NULL, " ");
         tok = strtok(NULL, " ");
         if (!strcmp(tok, "true")) {
            printf("%s was set to TRUE!\n", ifcond);
            actual_elim(blk, if_line, gparent, 1);
            (*changes)++;
         }
         else if (!strcmp(tok, "false")) {
            printf("%s was set to FALSE!\n", ifcond);
            actual_elim(blk, if_line, gparent, 0);
            (*changes)++;
         }
         break;
      }

      free(tmp);
   }
}

void find_ifs(struct bblk *blk, int *changes, struct funcs *froot) {
   struct line *l_if, *l_set, *l, *l2;
   struct bblk_child *child = blk->child;
   struct bblk *tblk;
   char reg[10];
   if (blk->visited)
      return;
   blk->visited = true;
   for (l=blk->lines; l; l=l->next) {
      if (!strncmp(l->text, "IF", 2)) {
         //printf("%s\n", l->text);

         // find where cfg branches
         sscanf(l->text, "IF %s = %*s", reg);
         for (tblk=froot->func; tblk; tblk=tblk->next) {
            // if assigning to if condition, that is bblk we need
            if (!strncmp(tblk->lines->text, reg, strlen(reg))) {
               // ensure has two children, otherwise already applied optimization
               if (tblk->child && tblk->child->next)
                  find_cond_consts(blk, l, tblk, changes);
               break;
            }
         }
      }
   }
   for (child = blk->child; child; child = child->next){
      if (child && child->id)
         find_ifs(child->id, changes, froot);
   }
}

void eliminate_unreachable_code(int *changes) {
   struct funcs *func;
   struct bblk_child *child;
   struct line *l;

   for (func = &cfgs; func; func = func->next)
      reset_nodes(func->func);

   for (func = &cfgs; func; func = func->next) {
      //printf("%s-----\n", func->func->lines->text);
      cfunc = func;
      find_ifs(func->func->child->id, changes, func);
   }

   for (func = &cfgs; func; func = func->next)
      reset_nodes(func->func);

   return;
}



void duplicate_branch_elimination(int *changes){
   struct funcs *func;
   struct bblk *cblk;
   struct line *cline;
   for (func = &cfgs; func; func = func->next) {
      cfunc = func;
      for (cblk = func->func; cblk; cblk = cblk->next) {
         //printf("cblk lines %s",cblk->lines->text);
         if (cblk->lines->text[0] == 'I' && cblk->lines->text[1] == 'F') {
            struct bblk *b1 = cblk->parent->id;
            struct bblk *b2 = cblk->parent->next->id;
            struct bblk *if_blk = b1->parent->id;
            struct line *line1;
            struct line *line2;
            char text_cpy[50];
            char dst_reg[6];
            char reg1[6];
            char reg2[6];
            char val1[6];
            char val2[6];
            char tmp[30];
            char tmp1[30];
            char tmp2[30];
            char *split;
            strcpy(text_cpy,cblk->lines->text);
            split = strchr(text_cpy,'n');
            split += 2;
            strcpy(tmp,split);
            strcpy(tmp1,split);
            split = strchr(tmp1,':');
            *split = '\0';
            strcpy(dst_reg,tmp1);
            //printf("%s\n",dst_reg);
            strcpy(tmp1,tmp);
            split = strchr(tmp1,'=');
            split += 2;
            strcpy(tmp1,split);
            strcpy(tmp2,tmp1);
            split = strchr(tmp1,',');
            *split = '\0';
            strcpy(reg1,tmp1);
            //printf("%s\n",reg1);
            split = strchr(tmp2,'=');
            split += 2;
            strcpy(reg2,split);
            //printf("%s\n",reg2);
            for (line1 = b1->lines; line1; line1 = line1->next) {
               if (strstr(line1->text,reg1) != NULL || strstr(line1->text,reg2) != NULL) {
                  //printf("%s\n",line1->text);
                  break;
               }
               if (strstr(line2->text,reg1) != NULL || strstr(line2->text,reg2) != NULL) {
                  //printf("%s\n",line2->text);
                  break;
               }
            }
            for (line2 = b2->lines; line2; line2 = line2->next) {
               if (strstr(line2->text,reg1) != NULL || strstr(line2->text,reg2) != NULL) {
                  //printf("%s\n",line2->text);
                  break;
               }
               if (strstr(line2->text,reg1) != NULL || strstr(line2->text,reg2) != NULL) {
                  //printf("%s\n",line2->text);
                  break;
               }
            }
            split = strchr(line1->text,'=');
            split += 2;
            strcpy(val1,split);
            //printf("%s\n",val1);
            split = strchr(line2->text,'=');
            split += 2;
            strcpy(val2,split);
            //printf("%s\n",val2);
            if (!strcmp(val1,val2)) {
               *changes = 1;
               remove_child(if_blk,b1);
               remove_child(if_blk,b2);
               add_child(if_blk,cblk);
               remove_parent(cblk,b1);
               remove_parent(cblk,b2);
               add_parent(cblk,if_blk);

               remove_bblk(cfunc,b1);
               remove_bblk(cfunc,b2);
               sprintf(cblk->lines->text,"%s := %s",dst_reg,val1);
            }
         }

      }
   }
   return;   
}
