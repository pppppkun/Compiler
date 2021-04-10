#include "semantic.h"

void DebugPrintNameType(ASTNode node)
{
    if (SEMANTIC_DEBUG)
        printf("%s %d\n", node.name, node.type);
}

void DebugPrintNameValue(ASTNode node)
{
    if (SEMANTIC_DEBUG)
        printf("%s %s\n", node.name, node.value);
}

int *GetSon(ASTNode node)
{
    int *sons = malloc(sizeof(int) * 10);
    int index = 0;
    sons[index++] = node.child;
    ASTNode child = nodes[sons[index - 1]];
    while (child.brother != -1)
    {
        sons[index++] = child.brother;
        child = nodes[sons[index - 1]];
    }
    // sons[index++] = nodes[node.child];
    // ASTNode child = sons[index-1];
    // while(child.brother!=-1){
    //     sons[index++] = nodes[child.brother];
    //     child = sons[index-1];
    // }
    return sons;
}

int SymbolHash(Symbol symbol)
{
    
}

unsignedinthash_pjw(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
            val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

int ProgramAnalyze(int index)
{
    ASTNode program = nodes[index];
    DebugPrintNameType(program);
    ASTNode child = nodes[program.child];
    do
    {
        // printf("%s %s %s\n", child.name, child.value, node.name);
        ExtDefListAnalyze(program.child);
    } while (child.brother != -1);
    return 0;
}

int ExtDefListAnalyze(int index)
{
    ASTNode ext_def_list = nodes[index];
    //type = 1 represent ExtDefList -> ExtDef ExtDefList
    if (ext_def_list.type == ExtDefList_ExtDefExtDefList)
    {
        int ext_def = ext_def_list.child;
        int ext_def_list = nodes[ext_def].brother;
        ExtDefAnalyze(ext_def);
        ExtDefListAnalyze(ext_def_list);
    }
    else
    {
    }
    return 0;
}

int ExtDefAnalyze(int index)
{
    ASTNode ext_def_node = nodes[index];
    DebugPrintNameType(ext_def_node);
    int child = ext_def_node.child;
    ASTNode child_node = nodes[child];
    switch (ext_def_node.type)
    {
    case ExtDef_SpecifierExtDecListSEMI:
        break;
    case ExtDef_SpecifierSEMI:
        SpecifierAnalyze(child);
        break;
    case ExtDef_SpecifierFunDecSEMI:
    {
        Type type = SpecifierAnalyze(child);
        type.value = RIGHT;
        FunDecAnalyze(child_node.brother);
        break;
    }
        // childNode = nodes[childNode.brother];
    case ExtDef_SpecifierFunDecCompSt:
        break;
    default:
        break;
    }
}

Type SpecifierAnalyze(int index)
{
    ASTNode specifier = nodes[index];
    DebugPrintNameType(specifier);
    printf("%s\n", "struct analyze");
    ASTNode spe_child = nodes[specifier.child];
    switch (specifier.type)
    {
    case Specifier_TYPE:
    {
        Type type;
        type.kind = BASIC;
        type.value = LEFT;
        int res = strcmp(spe_child.value, "int");
        if (res == 0)
            type.basic = INT;
        else
            type.basic = FLOAT;
        return type;
    }
    case Specifier_StructSpecifier:
    {
        StructAnalyze(specifier.child);
        break;
    }

    default:
        break;
    }
}

int StructAnalyze(int index)
{
    ASTNode struct_ = nodes[index];
    DebugPrintNameType(struct_);
    int *sons = GetSon(struct_);
    Type type;
    type.kind = STRUCTURE;
    type.value = LEFT;
    Field field;
    char *name = OptTagAnalyze(sons[1]);
    Symbol symbol;
    symbol.name = name;
    symbol.kind = STRUCTURE;
    symbol.type = type;
    DebugPrintNameValue(nodes[sons[0]]);
    DebugPrintNameType(nodes[sons[1]]);
    DebugPrintNameValue(nodes[sons[2]]);
    DebugPrintNameType(nodes[sons[3]]);
    DebugPrintNameValue(nodes[sons[4]]);
}

char *OptTagAnalyze(int index)
{
    ASTNode opt_tag = nodes[index];
    switch (opt_tag.type)
    {
    case OptTag_ID:
        return nodes[opt_tag.child].value;
    default:
        return ANONYMITY;
    }
}

int FunDecAnalyze(int index)
{
    ASTNode fun_dec = nodes[index];
}

int semanticAnalyze(int last_node)
{
    return ProgramAnalyze(last_node);
}