#include "codegen.h"

// void insert_reg(struct reg_map *map, char *reg){
//     if (!map)
// }

void get_virtual_regs(struct bblk *blk, struct reg_map *map){
    char reg[MAX_REG_LEN] = "\0", buf[MAX_LINE_SIZE];
    for (struct line *l = blk->lines; l; l = l->next){
        sscanf(l->text, "%s := %[^\t\n]", reg, buf);
        printf("%s\n", reg);
    }
    for (struct bblk_child *cc = blk->child; cc; cc = cc->next){
        get_virtual_regs(cc->id, map);
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
    // Virtual Registers
    extern struct funcs cfgs;
    struct reg_map *regs;
    for (struct funcs *f = &cfgs; f; f = f->next){
        get_virtual_regs(f->func->child->id, regs);
    }

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
        if (!strncmp(l->text, reg, strlen(reg))) return blk;
    }
    for (struct bblk_parent *p = blk->parent; p; p = p->next){
        struct bblk *result = find_definition(p->id, reg);
        if (result) return result;
    }
    return NULL;
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
            sscanf(l->text, "IF %s = %[^,], then %s := %[^,], else %s := %s", vi, t1, vd, vt, vd, ve);
            struct bblk *tblk = find_definition(blk, vt), *eblk = find_definition(blk, ve);
            fprintf(fp, "\t\tif (%s) goto bb%d; else goto bb%d;\n", vi, tblk->node->id, eblk->node->id);
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
    if (blk->child) fprintf(fp, "\t\tgoto bb%d;\n",blk->child->id->node->id);
    else fprintf(fp, "\t\treturn v%d;\n", blk->node->id);
    for (struct bblk_child *c = blk->child; c; c = c->next){
        generate_node_code(c->id, fp);
    }
}

void gen_func(struct bblk *root, FILE *fp) {
    struct table_entry *e = st_get_func(root->node->token);
    // printf("%s: %s\n", e->name, e->type ? "int" : "bool");
    char buf[MAX_LINE_SIZE] = "\0", text[MAX_LINE_SIZE] = "\0";
    sprintf(text, "%s %s (", e->type ? "int" : "bool", e->name);
    int first = 1;
    for (int i = 0; i < e->num_arg; i++){
        // printf("\t%s: %s\n", find_ast_node(e->args[i].id)->token, e->args[i].type ? "int" : "bool");
        if (first){
            sprintf(buf, "%s %s", "int", find_ast_node(e->args[i].id)->token);
            first = 0;
        } else sprintf(buf, ", %s %s", "int", find_ast_node(e->args[i].id)->token);
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