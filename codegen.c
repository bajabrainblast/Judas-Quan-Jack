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
    char varstr[200] = "";
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
    fprintf(fp, "int %s\n\n", varstr);
}

void gen_func(struct bblk *root, FILE *fp) {

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

    // for all non mains
    for(f=&cfgs; f; f=f->next) {
        if (strncmp(f->func->lines->text, "PEP", 3)) {
            printf("generating %s\n", f->func->lines->text);
            gen_func(f->func, fp);
        }
    }
    // for main
    for(f=&cfgs; f; f=f->next) {
        if (!strncmp(f->func->lines->text, "PEP", 3)) {
            printf("generating %s\n", f->func->lines->text);
            gen_main(f->func, fp);
            break;
        }
    }

    // close file
    fclose(fp);
    printf("done generating c code\n");
}