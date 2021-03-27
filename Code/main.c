#include "main.h"
int nums = 16;
void my_debug(char *str, int line)
{
    if (DEBUG)
    {
        printf("%s  %d\n", str, line);
    }
}

void AST_debug(char *father, char *flag, char *child_or_brother)
{
    if (AST_DEBUG)
    {
        printf("%s %s %s\n", father, flag, child_or_brother);
    }
}

int buildAST(char *name, int childSum, ...)
{
    ASTNode node;
    node.name = malloc(strlen(name));
    strcpy(node.name, name);
    node.lineno = -1;
    node.type = 0;
    node.brother = -1;
    node.child = -1;

    va_list child;
    va_start(child, childSum);
    if (childSum > 0)
    {
        int index = va_arg(child, int);
        node.lineno = nodes[index].lineno;
        node.child = index;
        AST_debug(name, "get child", nodes[index].name);
        for (int i = 1; i < childSum; i++)
        {
            nodes[index].brother = va_arg(child, int);
            // printf("%s get brother\n", nodes[index].name);
            AST_debug(nodes[index].name, "get brother", "");
            index = nodes[index].brother;
            AST_debug(name, "get child", nodes[index].name);
        }
    }
    else if (childSum == 0)
    {
        node.lineno = va_arg(child, int);
        node.value = malloc(strlen(yytext));
        strcpy(node.value, yytext);
        node.type = 1;
    }
    else
    {
        // printf("// %s %d//\n", node.name, node.lineno);
    }
    va_end(child);
    nodes[nodes_point++] = node;
    if(nodes_point > nums / 2){
        nums = nums * 2;
        ASTNode* nnodes = (ASTNode *)malloc(sizeof(ASTNode) * nums);
        // nodes = (ASTNode *)memcpy(nnodes, nodes, (nodes_point-1) * sizeof(ASTNode));
        for(int i = 0;i<nodes_point;i++){
            nnodes[i] = nodes[i];
        }
        free(nodes);
        nodes = nnodes;
        // printf("%d %d\n",nodes, nnodes);
        // for(int i = 0;i<nodes_point;i++){
        //     printf("%s %s \n", nodes[i].name, nodes[i].value);
        // }
    }
    return nodes_point - 1;
}

int print_AST(int now, int space)
{
    // printf("Enter Print AST %d\n",now);
    if (now < 0)
        return 0;
    ASTNode node = nodes[now];
    if (node.lineno != -1)
    {
        for (int i = 0; i < space; i++)
            printf("  ");
        // flex
        if (node.type == 1)
        {
            printf("%s", node.name);
            if (node.name[0] == 'I' && node.name[1] == 'D')
            {
                printf(": %s\n", node.value);
            }
            else if (node.name[0] == 'I' && node.name[1] == 'N')
            {
                printf(": %s\n", node.value);
            }
            else if (node.name[0] == 'F' && node.name[1] == 'L')
            {
                printf(": %s\n", node.value);
            }
            else if (node.name[0] == 'T' && node.name[1] == 'Y')
            {
                printf(": %s\n", node.value);
            }
            else
            {
                printf("\n");
            }
        }
        // bison
        else
        {
            printf("%s (%d)\n", node.name, node.lineno);
        }
    }
    print_AST(node.child, space + 1);
    print_AST(node.brother, space);
}

int main(int argc, char **argv)
{
    AST_PRINT_FLAG = 1;
    nodes = (ASTNode *)malloc(sizeof(ASTNode) * nums);
    // int index = buildAST("EPSILON", -1);
    // printf("%s %d", nodes[index].name, nodes[index].lineno);
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    // yydebug=1;
    yyparse();
    if (AST_PRINT_FLAG)
        print_AST(nodes_point - 1, 0);
    return 0;
}