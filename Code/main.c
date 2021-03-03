#include <stdio.h>
extern FILE* yyin;
extern int yylineno;
int main(int argc, char** argv){
    FILE *f = fopen(argv[1], "r");
    yyrestart(f);
    yylineno=1;
    yylex();
    fclose(f);
    return 0;
}