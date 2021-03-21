#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
//extern FILE* yyin;
/* deal with debug */
#define DEBUG 0
#define AST_DEBUG 0

/* deal with flex and bison */
void yyrestart(FILE*);
int yyparse();
char* yytext;
extern int yydebug;
extern int AST_PRINT_FLAG;

/* deal with debug mode */
void my_debug(char *str,int line);

/* deal with AST */
int buildAST(char* name, int childSum, ...);
int print_AST(int now, int space);
typedef struct ASTNode ASTNode;
struct ASTNode{
    char* name;
    int lineno;
    char* value;
    int child;
    int brother;
    int type;
};
ASTNode* nodes;
int nodes_point;