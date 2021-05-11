#include "main.h"
#include "semantic.h"
#include "ir.h"
long nums = 1000000;
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
    ASTNode *node = malloc(sizeof(ASTNode));
    if(name != NULL)
    {
        node->name = malloc(strlen(name)+1);
        strcpy(node->name, name);
    }
    node->lineno = -1;
    node->type = 1;
    node->brother = -1;
    node->child = -1;

    va_list child;
    va_start(child, childSum);
    if (childSum > 0)
    {
        int index = va_arg(child, int);
        node->lineno = nodes[index]->lineno;
        node->child = index;
        AST_debug(name, "get child", nodes[index]->name);
        for (int i = 1; i < childSum; i++)
        {
            nodes[index]->brother = va_arg(child, int);
            // printf("%s get brother\n", nodes[index].name);
            AST_debug(nodes[index]->name, "get brother", "");
            index = nodes[index]->brother;
            AST_debug(name, "get child", nodes[index]->name);
        }
        node->type = va_arg(child, int);
    }
    else if (childSum == 0)
    {
        node->lineno = va_arg(child, int);
        if(yytext!=NULL)
        {
            node->value = malloc(strlen(yytext)+1);
            strcpy(node->value, yytext);
        }
        node->type = 0;
    }
    else
    {
        node->type = -1;
        // printf("// %s %d//\n", node.name, node.lineno);
    }
    va_end(child);
    if (nodes_point > nums)
    {
        // printf("malloc! %ld -> %ld\n", nums, nums * 2);
        nums = nums * 2;
        nodes = realloc(nodes, nums);
        if (!nodes)
        {
            printf("Error!\n");
        }
    }
    nodes[nodes_point++] = node;
    // printf("nodes_nums:%d\n", nodes_point);
    return nodes_point - 1;
}

// int print_AST(int now, int space)
// {
//     // printf("Enter Print AST %d\n",now);
//     if (now < 0)
//         return 0;
//     ASTNode node = nodes[now];
//     if (node.type != -1)
//     {
//         for (int i = 0; i < space; i++)
//             printf("  ");
//         // flex
//         if (node.type == 0)
//         {
//             printf("%s", node.name);
//             if (node.name[0] == 'I' && node.name[1] == 'D')
//             {
//                 printf(": %s\n", node.value);
//             }
//             else if (node.name[0] == 'I' && node.name[1] == 'N')
//             {
//                 printf(": %s\n", node.value);
//             }
//             else if (node.name[0] == 'F' && node.name[1] == 'L')
//             {
//                 printf(": %s\n", node.value);
//             }
//             else if (node.name[0] == 'T' && node.name[1] == 'Y')
//             {
//                 printf(": %s\n", node.value);
//             }
//             else
//             {
//                 printf("\n");
//             }
//         }
//         // bison
//         else
//         {
//             printf("%s (%d)\n", node.name, node.lineno);
//         }
//     }
//     print_AST(node.child, space + 1);
//     print_AST(node.brother, space);
// }

int main(int argc, char **argv)
{
    AST_PRINT_FLAG = 1;
    nodes = malloc(sizeof(ASTNode *) * nums);
    if (argc <= 2)
    {
        printf("ERROR: need 2 file\n");
        return 1;
    }
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
    {
        //print_AST(nodes_point - 1, 0);
        semanticAnalyze(nodes_point - 1);
        gen_ir(nodes_point - 1, argv[2]);
    }
    return 0;
}