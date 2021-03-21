#include <stdio.h>
#include <string.h>
//extern FILE* yyin;
#define DEBUG 0

int yyrestart(FILE*);
int yyparse();
int buildAST(char* name, int childSum, ...);
extern int yydebug;
typedef struct ASTNode ASTNode;
struct ASTNode{
    char name[32];
    int lineno;
    char value[32];
    ASTNode* child;
    ASTNode* brother;
};