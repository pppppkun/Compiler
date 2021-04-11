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

void SemanticError(int typeno, int lineno, char *reason)
{
    printf("Error type %d at Line %d: %s\n", typeno, lineno, reason);
    //Error type 1 at Line 4: Undefined variable "j".
}

unsigned int hash_pjw(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~nodes_point)
            val = (val ^ (i >> 12)) & nodes_point;
    }
    return val;
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

int SymbolInsert(Symbol *symbol)
{
    char *name = symbol->name;
    unsigned int index = hash_pjw(name);
    symbol->hashcode = index;
    // first insert
    if (symbol_table[index]->hashcode == -1)
    {
        symbol_table[index] = symbol;
    }
    else
    {
        Symbol *node = symbol_table[index];
        while (node->next != NULL)
        {
            node = node->next;
        }
        node->next = symbol;
    }
    //TODO
}

int SymbolContains(char *name, SymbolKind kind)
{
    unsigned int index = hash_pjw(name);
    Symbol *symbol = symbol_table[index];
    if (symbol->hashcode == -1)
        return 0;
    /*
        if(strcmp(name, symbol.name)==0) return 1;
        if(symbol.next == NULL) break;
        else symbol = *symbol.next;    
    */
    while (1)
    {
        if (strcmp(name, symbol->name) == 0)
        {
            switch (symbol->kind)
            {
            case VAR:
                if (kind == VAR || kind == STRUCT)
                    return 1;
                break;
            case FIELD:
                if (kind == FIELD)
                    return 1;
                break;
            case STRUCT:
                if (kind == VAR || kind == STRUCT)
                    return 1;
                break;
            case FUNCION:
                break;
            default:
                break;
            }
        }
    }
    return 0;
}

Symbol *SymbolGet(char *name, SymbolKind kind)
{
    // return &symbol_table[index];
    unsigned int index = hash_pjw(name);
    Symbol *symbol = symbol_table[index];
    for (;;)
    {
        if (strcmp(name, symbol->name) == 0 && symbol->kind == kind)
        {
            return symbol;
        }
        else
        {
            if (symbol->next == NULL)
                break;
            symbol = symbol->next;
        }
    }
    return NULL;
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
    Type* type = malloc(sizeof(Type));
    switch (ext_def_node.type)
    {
    case ExtDef_SpecifierExtDecListSEMI:
        SpecifierAnalyze(child, type);
        break;
    case ExtDef_SpecifierSEMI:
        SpecifierAnalyze(child, type);
        break;
    case ExtDef_SpecifierFunDecSEMI:
    {
        SpecifierAnalyze(child, type);
        type->value = RIGHT;
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

int SpecifierAnalyze(int index, Type* type)
{
    ASTNode specifier = nodes[index];
    DebugPrintNameType(specifier);
    ASTNode spe_child = nodes[specifier.child];
    switch (specifier.type)
    {
    case Specifier_TYPE:
    {
        type->kind = BASIC;
        type->value = LEFT;
        int res = strcmp(spe_child.value, "int");
        if (res == 0)
            type->basic = INT;
        else
            type->basic = FLOAT;
        return 1;
    }
    case Specifier_StructSpecifier:
    {
        StructAnalyze(specifier.child, type);
        break;
    }
    default:
        break;
    }
}

int StructAnalyze(int index, Type* type)
{
    ASTNode struct_ = nodes[index];
    DebugPrintNameType(struct_);
    int *sons = GetSon(struct_);
    switch (struct_.type)
    {
    case StructSpecifier_STRUCTOptTagLCDefListRC:
    {
        char *name = OptTagAnalyze(sons[1]);
        type->kind = STRUCTURE;
        type->value = LEFT;
        if (SymbolContains(name, STRUCT))
        {
            SemanticError(16, struct_.lineno, "duplicate with the name of a structure or variable");
        }
        else
        {
            Symbol *symbol = malloc(sizeof(Symbol));
            symbol->name = name;
            symbol->kind = STRUCT;
            symbol->type = type;
            symbol->next = NULL;
            Field* field = malloc(sizeof(Field));
            DefListAnalyze(sons[3], field, STRUCT);
            SymbolInsert(symbol);
        }
        break;
    }
    case StructSpecifier_STRUCTTag:
    {
        char *name = TagAnalyze(sons[1]);
        if (SymbolContains(name, STRUCT) == 0)
        {
            SemanticError(17, struct_.lineno, "using structure to def var before declare");
        }
        else
        {
            Symbol *symbol = SymbolGet(name, STRUCT);
            type = symbol->type;
            // printf("%s %d\n", symbol->name, type->kind);
            return 1;
        }
    }
    default:
        break;
    }
}

int DefListAnalyze(int index, Field* field, SymbolKind kind){
    ASTNode def_list = nodes[index];
    int* sons = GetSon(def_list);
    DebugPrintNameType(def_list);
    switch (def_list.type)
    {
    case DefList_DefDefList:
        DefAnalyze(sons[0], field, kind);
        DefListAnalyze(sons[1], field, kind);
        break;
    default:
        break;
    }
}

int DefAnalyze(int index, Field* field, SymbolKind kind){
    ASTNode def = nodes[index];
    int* sons = GetSon(def);
    DebugPrintNameType(def);
    Type* type = malloc(sizeof(type));
    SpecifierAnalyze(sons[0], type);
    switch (kind)
    {
    case VAR:
        break;
    case STRUCT:
        
        break;
    default:
        break;
    }
    // printf("%d\n", type->kind);
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

char *TagAnalyze(int index)
{
    ASTNode tag = nodes[index];
    return nodes[tag.child].value;
}

int FunDecAnalyze(int index)
{
    ASTNode fun_dec = nodes[index];
}

int semanticAnalyze(int last_node)
{
    symbol_table = malloc(sizeof(Symbol *) * nodes_point);
    for (int i = 0; i < nodes_point; i++)
    {
        symbol_table[i] = malloc(sizeof(Symbol *) * nodes_point);
        symbol_table[i]->hashcode = -1;
    }
    return ProgramAnalyze(last_node);
}