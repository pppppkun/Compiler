#include <stdio.h>
#include "global.h"
//extern FILE* yyin;
#define DEBUG 0
int yyrestart(FILE*);
int yyparse();
void my_debug(char *str,int line){
    if(DEBUG){
        printf("%s  %d\n", str, line);
    }
}

extern int yydebug;
int main(int argc, char** argv){
    if(argc <= 1) return 1;
    FILE *f = fopen(argv[1], "r");
    if(!f){
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    // yydebug=1;
    yyparse();
    return 0;
}