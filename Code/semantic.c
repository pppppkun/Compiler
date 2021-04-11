#include "semantic.h"

void DebugPrintNameType(ASTNode node)
{
    if (SEMANTIC_DEBUG == 1)
        printf("%s %d\n", node.name, node.type);
}

void DebugPrintNameValue(ASTNode node)
{
    if (SEMANTIC_DEBUG == 1)
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
        if (i = val & ~(nodes_point-1))
            val = (val ^ (i >> 12)) & (nodes_point-1);
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
    printf("%d\n", index);
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
    printf("get symbol in contains %d\n", index);
    if (symbol->hashcode == -1)
        return 0;
    while (symbol != NULL)
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
        symbol=symbol->next;
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
    Type *type = malloc(sizeof(Type));
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

int SpecifierAnalyze(int index, Type *type)
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

int StructAnalyze(int index, Type *type)
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
            Field *field = malloc(sizeof(Field));
            type->field = field;
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

int DefListAnalyze(int index, Field *field, SymbolKind kind)
{
    ASTNode def_list = nodes[index];
    int *sons = GetSon(def_list);
    DebugPrintNameType(def_list);
    Field *field_def = malloc(sizeof(Field));
    field->next = field_def;
    switch (def_list.type)
    {
    case DefList_DefDefList:
        DefAnalyze(sons[0], field_def, kind);
        DefListAnalyze(sons[1], field_def, kind);
        break;
    default:
        break;
    }
}

int DefAnalyze(int index, Field *field, SymbolKind kind)
{
    ASTNode def = nodes[index];
    int *sons = GetSon(def);
    DebugPrintNameType(def);
    Type *type = malloc(sizeof(type));
    SpecifierAnalyze(sons[0], type);
    switch (kind)
    {
    case VAR:
        break;
    case STRUCT:
        // field = malloc(sizeof(Field));
        type->field = field;
        DecListAnalyze(sons[1], type, kind);
        field->type = type;
        type->field = NULL;
        break;
    default:
        break;
    }
    // printf("%d\n", type->kind);
}

int FunDecAnalyze(int index)
{
    ASTNode fun_dec = nodes[index];
}

int DecListAnalyze(int index, Type *type, SymbolKind kind)
{
    ASTNode dec_list = nodes[index];
    DebugPrintNameType(dec_list);
    int *sons = GetSon(dec_list);
    // DecAnalyze(sons[0], type, kind);
    switch (dec_list.type)
    {
    case DecList_Dec:
        DecAnalyze(sons[0], type, kind);
        break;
    case DecList_DecCOMMADecList:
        DecAnalyze(sons[0], type, kind);
        DecListAnalyze(sons[2], type, kind);
    default:
        break;
    }
}

int DecAnalyze(int index, Type *type, SymbolKind kind)
{
    ASTNode dec = nodes[index];
    DebugPrintNameType(dec);
    int *sons = GetSon(dec);
    // VarDecAnalyze(dec.child, type, kind);
    switch (dec.type)
    {
    case Dec_VarDec:
        VarDecAnalyze(sons[0], type, kind);
        break;
    case Dec_VarDecASSIGNOP_Exp:
        if (kind == STRUCT)
        {
            SemanticError(15, dec.lineno, "Initializes on the field of the structure");
        }
        break;
    default:
        break;
    }
}

int VarDecAnalyze(int index, Type *type, SymbolKind kind)
{
    ASTNode var_dec = nodes[index];
    DebugPrintNameType(var_dec);
    int *sons = GetSon(var_dec);
    switch (var_dec.type)
    {
    case VarDec_ID:
        // printf("%s \n",nodes[sons[0]].value);
        {
            char *name = nodes[sons[0]].value;
            if (SymbolContains(name, kind) == 1)
            {
                if (kind == VAR)
                    SemanticError(3, var_dec.lineno, "Redefine Variable");
                if (kind == STRUCT)
                    SemanticError(15, var_dec.lineno, "Redefine Field");
            }
            else
            {
                Symbol *symbol = malloc(sizeof(Symbol));
                symbol->name=name;
                symbol->next=NULL;
                if (kind == VAR)
                {
                    symbol->name = name;
                    symbol->kind = VAR;
                    symbol->type = type;
                    symbol->next = NULL;
                }
                if (kind == STRUCT)
                {
                    Field *field = type->field;
                    type->field = NULL;
                    field->type = type;
                    field->name = name;
                    field->next = malloc(sizeof(Field));
                    symbol->kind=FIELD;
                    symbol->name=name;
                    symbol->type= malloc(sizeof(Type));
                    // symbol->type->kind=type->kind;
                    // symbol->type->value=LEFT;
                    // symbol->type->field=field;
                    type->field = field->next;
                }
                SymbolInsert(symbol);
                printf("INSERT SUCCESS!\n");
            }
            break;
        }
    case VarDec_LBINTRB:

        break;
    default:
        break;
    }
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