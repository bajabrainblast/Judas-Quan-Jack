#include "codegen.h"

// void insert_reg(struct reg_map *map, char *reg){
//     if (!map)
// }

int declared(struct reg_map *list, char *var){
    printf("Checking %s:\n", var);
    struct reg_map *r;
    for (r = list; r; r = r->next){
        printf("\t%s\n", r->reg);
        if (!strcmp(r->reg, var)) return 1;
    }
    return 0;
}

void insert_var(struct reg_map **list, struct reg_map *var){
    printf("Inserting %s\n", var->reg);
    if (!(*list)) {
        *list = var;
        return;
    }
    struct reg_map *r;
    for (r = *list; r->next; r = r->next);
    r->next = var;
}

void print_var(char *reg, int *first, FILE *fp){
        if (*first){
            fprintf(fp, "%s", reg);
            *first = 0;
        } else fprintf(fp, ", %s", reg);
}

void get_virtual_regs(struct bblk *blk, int *first, FILE *fp){
    char reg[MAX_REG_LEN] = "\0", buf[MAX_LINE_SIZE];
    for (struct line *l = blk->lines; l; l = l->next){
        if (strstr(l->text, "IF")){
            char buf2[MAX_LINE_SIZE];
            sscanf(l->text, "%[^,], then %s %[^\t\n]", buf, reg, buf2);
            print_var(reg, first, fp);
            continue;
        }
        sscanf(l->text, "%s := %[^\t\n]", reg, buf);
        print_var(reg, first, fp);
    }
    for (struct bblk_child *cc = blk->child; cc; cc = cc->next){
        get_virtual_regs(cc->id, first, fp);
    }
}

void gen_header(FILE *fp) {
    // headers
    fprintf(fp, "#include <stdio.h>\n#include <stdlib.h>\n\n");

    // get_bool
    fprintf(fp, "int get_bool() {\n");
    fprintf(fp, "\tint tmp;\n");
    fprintf(fp, "\tprintf(\"Enter 0 for FALSE or 1 for TRUE\\n\");\n");
    fprintf(fp, "\tscanf(\"%%d\", &tmp);\n");
    fprintf(fp, "\tif (tmp != 0 && tmp != 1) exit(1);\n");
    fprintf(fp, "\treturn tmp;\n");
    fprintf(fp, "}\n\n");

    // get_int
    fprintf(fp, "int get_int() {\n");
    fprintf(fp, "\tint tmp;\n");
    fprintf(fp, "\tprintf(\"Enter INT\\n\");\n");
    fprintf(fp, "\tscanf(\"%%d\", &tmp);\n");
    fprintf(fp, "\treturn tmp;\n");
    fprintf(fp, "}\n\n");
    
    // vars
    fprintf(fp, "int ");
    // Function arguments, local variables, and virtual registers
    extern struct funcs cfgs;
    struct funcs *f;
    struct reg_map *vars = NULL;
    int first = 1;
    for (f = &cfgs; f; f = f->next){
        struct table_entry *e = st_get_func(f->func->node->token);
        for (int i = 0; i < e->num_arg; i++){
            char *token = find_ast_node(e->args[i].id)->token;
            if (declared(vars, token)) continue;
            print_var(token, &first, fp);
            insert_var(&vars, rm_create(token, NULL));
        }
        get_virtual_regs(f->func->child->id, &first, fp);
    }
    fprintf(fp, ";\n\n");

    // char varstr[200] = "\0";
    // extern struct sym_table table;
    // struct table_entry *en;
    // int i;
    // for (en=table.start; en; en=en->next) {
    //     printf("here %s\n", en->name);
    //     if (!en->is_func && !(isArithematicConst(en->name) || isBooleanConst(en->name))) {
    //         strcat(varstr, en->name);
    //         strcat(varstr, ", ");
    //     }
    // }
    // for (i=199; i>=0; i--) {
    //     if (varstr[i] == ',') {
    //         varstr[i] = ';';
    //         break;
    //     }
    // }
    // if (varstr[0] != '\0') fprintf(fp, "int %s\n\n", varstr);
}

struct bblk *find_definition(struct bblk *blk, char reg[MAX_REG_LEN]){
    for (struct line *l = blk->lines; l; l = l->next){
        if (strstr(l->text, "IF")){
            char b1[MAX_LINE_SIZE] = "\0", b2[MAX_LINE_SIZE] = "\0", vi[MAX_REG_LEN] = "\0";
            printf("|%s|\n", l->text);
            sscanf(l->text, "%[^,], then %s %[^\t\n]", b1, vi, b2);
            printf("Match %s with %s\n", reg, vi);
            if (!strcmp(vi, reg)) return blk;
        }
        if (!strncmp(l->text, reg, strlen(reg))) return blk;
    }
    for (struct bblk_parent *p = blk->parent; p; p = p->next){
        struct bblk *result = find_definition(p->id, reg);
        if (result) return result;
    }
    for (struct bblk_parent *p = blk->parent; p; p = p->next){
        struct bblk *result = find_definition(p->id, reg);
        if (result) return result;
    }
    return NULL;
}

void dfs(struct bblk *left, struct bblk *left_prev, int *branch_count, int *left_is_then, char **cond_reg) {
   bool match_if = 0;
   for (struct line *l = left->lines; l; l = l->next) {
      if (strstr(l->text, "IF")){
         if ((*branch_count) == 1) {
            char vi[MAX_REG_LEN], vd[MAX_REG_LEN], vt[MAX_REG_LEN], ve[MAX_REG_LEN];
            char buf[MAX_LINE_SIZE] = "\0", t1[MAX_LINE_SIZE / 2 - 1] = "\0", t2[MAX_LINE_SIZE / 2 - 1] = "\0";
            char def_line_vd[MAX_REG_LEN];
            match_if = true;
            struct line *def_line;
            for (def_line = left_prev->lines; def_line->next; def_line = def_line->next);
            if (strstr(def_line->text, "IF")) {
               sscanf(def_line->text, "IF %s = %[^,], then %s := %[^,], else %s := %s", vi, t1, vd, vt, vd, ve);
            }
            else {
               sscanf(def_line->text, "%s := %[^\n]", vd, buf);
            }
            strcpy(def_line_vd,vd);
            sscanf(def_line->text, "IF %s = %[^,], then %s := %[^,], else %s := %s", vi, t1, vd, vt, vd, ve);
            strcpy((*cond_reg),vi);
            if (strcmp(def_line_vd,vt)) {
               (*left_is_then) = 1;
            }
            else {
               (*left_is_then) = 0;
            }
         }
         else {
            (*branch_count) = (*branch_count) - 1;
         }
      }
      else {
         continue;
      }
   }
   if (left->child && left->child->next) {
      (*branch_count) = (*branch_count) + 1;
   }
   if (!match_if) {
      dfs(left->child->id,left,branch_count,left_is_then,cond_reg);
   }
}

bool is_conditional_blk(struct bblk *cblk) {
   for (struct line *l = cblk->lines; l; l = l->next) {
      if (strstr(l->text, "IF")) {
         return true;
      }
   }
   return false;
}

void generate_node_code(struct bblk *blk, FILE *fp){
    if (blk->visited) return;
    blk->visited = true;
    char buf[MAX_LINE_SIZE] = "\0", t1[MAX_LINE_SIZE / 2 - 1] = "\0", t2[MAX_LINE_SIZE / 2 - 1] = "\0";
    // Make block label
    fprintf(fp, "\tbb%d:\n", blk->node->id);
    // Generate code inside block
    for (struct line *l = blk->lines; l; l = l->next){
        char vi[MAX_REG_LEN], vd[MAX_REG_LEN], vt[MAX_REG_LEN], ve[MAX_REG_LEN];
        if (strstr(l->text, "IF")){
            // THIS NEEDS TO BE UPDATED TO USE GRAPH REACHABILITY ALGORITHM FROM CLASS
            /*
            sscanf(l->text, "IF %s = %[^,], then %s := %[^,], else %s := %s", vi, t1, vd, vt, vd, ve);
            struct bblk *tblk = find_definition(blk, vt), *eblk = find_definition(blk, ve);
            fprintf(fp, "\t\tif (%s) goto bb%d; else goto bb%d;\n", vi, tblk->node->id, eblk->node->id);
            */
        } else {
            printf("%s\n",l->text);
            sscanf(l->text, "%s := %[^\n]", vd, buf);
            if (!strcmp(buf, "false")) sprintf(buf, "0");
            if (!strcmp(buf, "true")) sprintf(buf, "1");
            if (!strcmp(buf, "getbool")) sprintf(buf, "get_bool()");
            if (!strcmp(buf, "getint")) sprintf(buf, "get_int()");
            if (strstr(buf, "mod")){
                sscanf(buf, "%s mod %s", vd, vi);
                sprintf(buf, "%s %% %s", vd, vi);
            }
            if (strstr(buf, "div")){
                sscanf(buf, "%s div %s", vd, vi);
                sprintf(buf, "%s / %s", vd, vi);
            }
            if (strstr(buf, "not")){
                sscanf(buf, "not %s", t1);
                sprintf(buf, "!%s", t1);
            }
            while (strstr(buf, "and")){
                strncpy(t1, buf, (strstr(buf, "and") - buf) - 1);
                strcpy(t2, strstr(buf, "and") + 4);
                printf("%s - %s\n", t1, t2);
                sprintf(buf, "%s && %s", t1, t2);
            }
            while (strstr(buf, "or")){
                strncpy(t1, buf, (strstr(buf, "or") - buf) - 1);
                strcpy(t2, strstr(buf, "or") + 3);
                sprintf(buf, "%s && %s", t1, t2);
            }
            fprintf(fp, "\t\t%s = %s;\n", vd, buf);
        }
    }
    if (blk->child && blk->child->next) {
         struct bblk *left = blk->child->id;
         struct bblk *right = blk->child->next->id;
         char cond_reg[MAX_REG_LEN];
         int branch_count = 1;
         int left_is_then = 0;
         dfs(left,NULL,&branch_count,&left_is_then,&cond_reg);
         if (left_is_then) {
            fprintf(fp, "\t\tif (%s) goto bb%d;\n",cond_reg,left->node->id);
            fprintf(fp, "\t\telse goto bb%d;\n",right->node->id);
         }
         else {
            fprintf(fp, "\t\tif (%s) goto bb%d;\n",cond_reg,right->node->id);
            fprintf(fp, "\t\telse goto bb%d;\n",left->node->id);
         }
    }
    else if (blk->child) {
      if (is_conditional_blk(blk->child->id)) {
         char vi[MAX_REG_LEN], vd[MAX_REG_LEN], vt[MAX_REG_LEN], ve[MAX_REG_LEN];
         struct line *if_line;
         struct line *clineend;
         for (clineend = blk->lines; clineend->next; clineend = clineend->next);
         for (if_line = blk->child->id->lines; if_line; if_line = if_line->next) {
            if (strstr(if_line->text, "IF")) {
               break;
            }
         }
         sscanf(if_line->text, "IF %s = %[^,], then %s := %[^,], else %s := %s", vi, t1, vd, vt, vd, ve);
         if (strstr(clineend->text,vt)) {
            fprintf(fp, "\t\t%s = %s;\n", vd,vt);
         }
         else {
            fprintf(fp, "\t\t%s = %s;\n", vd,ve);
         }
      }
      fprintf(fp, "\t\tgoto bb%d;\n",blk->child->id->node->id);
    }
    else fprintf(fp, "\t\treturn v%d;\n", blk->node->id);
    for (struct bblk_child *c = blk->child; c; c = c->next){
        generate_node_code(c->id, fp);
    }
}

void gen_func(struct bblk *root, FILE *fp) {
    struct table_entry *e = st_get_func(root->node->token);
    // printf("%s: %s\n", e->name, e->type ? "int" : "bool");
    char buf[MAX_LINE_SIZE] = "\0", text[MAX_LINE_SIZE] = "\0";
    sprintf(text, "int %s (", e->name);
    int first = 1;
    for (int i = 0; i < e->num_arg; i++){
        // printf("\t%s: %s\n", find_ast_node(e->args[i].id)->token, e->args[i].type ? "int" : "bool");
        if (first){
            sprintf(buf, "int %s", find_ast_node(e->args[i].id)->token);
            first = 0;
        } else sprintf(buf, ", int %s", find_ast_node(e->args[i].id)->token);
        strcat(text, buf);
    }
    strcat(text, ") {");
    fprintf(fp, "%s\n", text);
    generate_node_code(root->child->id, fp);
    fprintf(fp, "}\n\n");
}

void gen_main(struct bblk *root, FILE *fp) {
    fprintf(fp, "int main(void){ return 0; }\n");
}

void generate_c_code() {
    extern struct funcs cfgs;
    struct bblk *blk;
    struct funcs *f;
    FILE *fp;

    printf("start generating c code\n");

    // open file
    fp = fopen("output.c", "w");

    // handle getint, getbool,
    // virtual regs, func args, and local vars
    gen_header(fp);
    
    for (f = &cfgs; f->next; f = f->next);
    for (f; f; f = f->prev){
        printf("Generating %s\n", f->func->lines->text);
        f == &cfgs ? gen_main(f->func, fp) : gen_func(f->func, fp);
    }
    // // for all non mains
    // for(f=&cfgs; f; f=f->next) {
    //     if (strncmp(f->func->lines->text, "PEP", 3)) {
    //         printf("generating %s\n", f->func->lines->text);
    //         gen_func(f->func, fp);
    //     }
    // }
    // // for main
    // for(f=&cfgs; f; f=f->next) {
    //     if (!strncmp(f->func->lines->text, "PEP", 3)) {
    //         printf("generating %s\n", f->func->lines->text);
    //         gen_main(f->func, fp);
    //         break;
    //     }
    // }

    // close file
    fclose(fp);
    printf("done generating c code\n");
}
