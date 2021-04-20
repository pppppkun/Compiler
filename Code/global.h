#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef _ASTNODE_
#define _ASTNODE_
typedef struct ASTNode ASTNode;
struct ASTNode{
    char* name;
    int lineno;
    char* value;
    int child;
    int brother;
    int type;//mark terminator or not
};
ASTNode** nodes;
int nodes_point;
#endif