#include <stdio.h>
#include <string.h>
//extern FILE* yyin;
#define DEBUG 0

int yyrestart(FILE*);
int yyparse();

extern int yydebug;

struct ASTNode{
    char[32] name;
    int lineno;
    char[32] value;
    ASTNode* child;
    ASTNode* brother;
};