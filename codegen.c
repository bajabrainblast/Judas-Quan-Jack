#include "codegen.h"

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
    fprintf(fp, "}\n");

    // get_int
    fprintf(fp, "int get_int() {\n");
    fprintf(fp, "\tint tmp;\n");
    fprintf(fp, "\tprintf(\"Enter INT\\n\");\n");
    fprintf(fp, "\tscanf(\"%%d\", &tmp);\n");
    fprintf(fp, "\treturn tmp;\n");
    fprintf(fp, "}\n\n");

    // vars
    char varstr[200] = "\0";
    extern struct sym_table table;
    struct table_entry *en;
    int i;
    for (en=table.start; en; en=en->next) {
        printf("here %s\n", en->name);
        if (!en->is_func && !(isArithematicConst(en->name) || isBooleanConst(en->name))) {
            strcat(varstr, en->name);
            strcat(varstr, ", ");
        }
    }
    for (i=199; i>=0; i--) {
        if (varstr[i] == ',') {
            varstr[i] = ';';
            break;
        }
    }
    if (varstr[0] != '\0') fprintf(fp, "int %s\n\n", varstr);
}

void generate_node_code(struct bblk *blk){
    printf("\tbb%d:\n", blk->node->id);
    for (struct line *l = blk->lines; l; l = l->next){
        printf("\t\t%s\n", l->text);
        // if (strstr(l->text, "IF")){
        //     printf("%s\n", l->text);
        // } else {
        //     printf("%s\n", l->text);
        // }
    }
    if (blk->child) printf("\t\tgoto bb%d\n",blk->child->id->node->id);
    for (struct bblk_child *c = blk->child; c; c = c->next){
        generate_node_code(c->id);
    }
}

void gen_func(struct bblk *root, FILE *fp) {
    struct table_entry *e = st_get_func(root->node->token);
    printf("%s: %s\n", e->name, e->type ? "int" : "bool");
    char buf[MAX_LINE_SIZE] = "\0", text[MAX_LINE_SIZE] = "\0";
    sprintf(text, "%s %s (", e->type ? "int" : "bool", e->name);
    // char  = e->type ? "int" : "bool";
    // strcat(text, "")
    int first = 1;
    for (int i = 0; i < e->num_arg; i++){
        printf("\t%s: %s\n", find_ast_node(e->args[i].id)->token, e->args[i].type ? "int" : "bool");
        if (first){
            sprintf(buf, "%s %s", e->args[i].type ? "int" : "bool", find_ast_node(e->args[i].id)->token);
            first = 0;
        } else sprintf(buf, ", %s %s", e->args[i].type ? "int" : "bool", find_ast_node(e->args[i].id)->token);
        strcat(text, buf);
    }
    strcat(text, ") {\n");
    printf("%s", text);
    generate_node_code(root->child->id);
}

void gen_main(struct bblk *root, FILE *fp) {

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