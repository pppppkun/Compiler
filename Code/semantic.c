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

void SemanticError(int typeno, int lineno, char *reason, char *addition)
{
    printf("Error type %d at Line %d: %s about %s\n", typeno, lineno, reason, addition);
    //Error type 1 at Line 4: Undefined variable "j".
}

char* random()
{
    char* name = malloc(sizeof(char)*10);
    for(int i = 0;i<10;i++)
    {
        int t = rand() % (10+26+26);
        if(t < 10) name[i] = (char)('0' + t);
        else if(t < 10 + 26) name[i] = (char)('a' + t - 10);
        else name[i] = (char)('A' + t - 10 - 26);
    }
    return name;
}

int TypeKindEqual(Type *t1, Type *t2)
{
    //TODO
    if (t1->kind != t2->kind)
        return 0;
    switch (t1->kind)
    {
    case BASIC:
        if (t1->basic == t2->basic)
            return 1;
        else
            return 0;
    case ARRAY:
        //TODO
        break;
    case STRUCTURE:
        //TODO
        break;
    default:
        break;
    }
}

unsigned int hash_pjw(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~(nodes_point - 1))
            val = (val ^ (i >> 12)) & (nodes_point - 1);
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
    return index;
    //TODO
}

int SymbolContains(char *name, SymbolKind kind)
{
    unsigned int index = hash_pjw(name);
    Symbol *symbol = symbol_table[index];
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
                if (kind == STRUCT || kind == VAR)
                    return 1;
                break;
            case STRUCT:
                if (kind == VAR || kind == STRUCT)
                    return 1;
                break;
            case FUNCION:
                if (kind == FUNCION)
                    return 1;
                break;
            default:
                break;
            }
        }
        symbol = symbol->next;
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
    // printf("%s %s %s\n", child.name, child.value, node.name);
    ExtDefListAnalyze(program.child);
    for (int i = 0; i < nodes_point; i++)
    {
        Symbol *symbol = symbol_table[i];
        if (symbol->hashcode == -1)
            continue;
        else
        {
            while (symbol != NULL)
            {
                char *kind;
                switch (symbol->kind)
                {
                case VAR:
                    kind = "var";
                    break;
                case FIELD:
                    kind = "field";
                    break;
                case STRUCT:
                    kind = "struct";
                    break;
                case FUNCION:
                    kind = "function";
                    break;
                default:
                    break;
                }
                printf("%s %s\n", kind, symbol->name);
                if(symbol->kind == STRUCT || (symbol->kind==VAR && symbol->type->kind==STRUCTURE))
                {
                    Field* field = symbol->type->field;
                    while(field!=NULL)
                    {
                        char* field_kind;
                        switch (field->type->kind)
                        {
                        case BASIC: 
                            if(field->type->basic==INT) field_kind = "int";
                            else field_kind ="float";
                            break;
                        case ARRAY:
                            field_kind = "array";
                            break;
                        case STRUCTURE:
                            field_kind = "structure";
                            break;
                        default:
                            break;
                        }
                        printf("\t%s %s %s.%s\n", "field",field_kind , symbol->name,field->name);
                        field = field->next;
                    }
                }
                if(symbol->kind==VAR && symbol->type->kind==ARRAY)
                {
                    Type* type = symbol->type;
                    while(type->kind == ARRAY)
                    {
                        printf("\tarray size is %d\n", type->array->size);
                        type = type->array->type;
                    }
                    printf("\tfinal type is %d\n", type->kind);
                }
                symbol = symbol->next;
            }
        }
    }
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
        return -1;
    return 0;
}

int ExtDefAnalyze(int index)
{
    ASTNode ext_def_node = nodes[index];
    DebugPrintNameType(ext_def_node);
    // int child = ext_def_node.child;
    int *sons = GetSon(ext_def_node);
    ASTNode child_node = nodes[sons[0]];
    Type *type = malloc(sizeof(Type));
    switch (ext_def_node.type)
    {
    case ExtDef_SpecifierExtDecListSEMI:
        SpecifierAnalyze(sons[0], type);
        ExtDecListAnalyze(sons[1], type);
        break;
    case ExtDef_SpecifierSEMI:
        SpecifierAnalyze(sons[0], type);
        break;
    case ExtDef_SpecifierFunDecSEMI:
        SpecifierAnalyze(sons[0], type);
        type->value = RIGHT;
        FunDecAnalyze(sons[1]);
        break;
    case ExtDef_SpecifierFunDecCompSt:
        SpecifierAnalyze(sons[0], type);
        type->value = RIGHT;
        FunDecAnalyze(sons[1]);
        CompStAnalyze(sons[2]);
        break;
    default:
        break;
    }
}

int FunDecAnalyze(int index)
{
    ASTNode fun_dec = nodes[index];
    DebugPrintNameType(fun_dec);
    int *sons = GetSon(fun_dec);
    char *name = nodes[sons[0]].value;
    if (SymbolContains(name, FUNCION) == 1)
    {
        SemanticError(4, fun_dec.lineno, "Redefined function", name);
        return 0;
    }
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->kind = FUNCION;
    symbol->name = name;
    symbol->next = NULL;
    switch (fun_dec.type)
    {
    case FunDec_IDLPVarListRP:
        VarListAnalyze(sons[2]);
        break;
    case FunDec_IDLPRP:
        SymbolInsert(symbol);
        break;
    default:
        break;
    }
}

int VarListAnalyze(int index)
{
    ASTNode var_list = nodes[index];
    DebugPrintNameType(var_list);
    int *sons = GetSon(var_list);
    switch (var_list.type)
    {
    case VarList_ParamDecCOMMAVarList:
        ParamDecAnalyze(sons[0]);
        VarListAnalyze(sons[2]);
        break;
    case VarList_ParamDec:
        ParamDecAnalyze(sons[0]);
        break;
    default:
        break;
    }
}

int ParamDecAnalyze(int index)
{
    ASTNode param_dec = nodes[index];
    DebugPrintNameType(param_dec);
    int *sons = GetSon(param_dec);
    Type *type = malloc(sizeof(Type));
    SpecifierAnalyze(sons[0], type);
    VarDecAnalyze(sons[1], type, NULL, VAR);
}

int CompStAnalyze(int index)
{
    ASTNode comp_st = nodes[index];
    DebugPrintNameType(comp_st);
    int *sons = GetSon(comp_st);
    DefListAnalyze(sons[1], NULL, VAR);
    StmtListAnalyze(sons[2]);
}

int StmtListAnalyze(int index)
{
    ASTNode stmt_list = nodes[index];
    DebugPrintNameType(stmt_list);
    if (stmt_list.type == StmtList_StmtStmtList)
    {
        int *sons = GetSon(stmt_list);
        StmtAnalyze(sons[0]);
        StmtListAnalyze(sons[1]);
    }
}

int StmtAnalyze(int index)
{
    ASTNode stmt = nodes[index];
    int *sons = GetSon(stmt);
    DebugPrintNameType(stmt);
    switch (stmt.type)
    {
    case Stmt_ExpSEMI:
        ExpAnalyze(sons[0]);
        break;
    case Stmt_CompSt:
        CompStAnalyze(sons[0]);
    case Stmt_ReturnExpSEMI:
        ExpAnalyze(sons[1]);
        break;
    case Stmt_IfLpExpRpStmt:
        //todo sons 2
        ExpAnalyze(sons[2]);
        StmtAnalyze(sons[4]);
        break;
    case Stmt_IfLpExpRpStmtElseStmt:
        //TODO 2 4 6
        ExpAnalyze(sons[2]);
        StmtAnalyze(sons[4]);
        StmtAnalyze(sons[6]);
        break;
    case Stmt_WhileLpExpRpStmt:
        ExpAnalyze(sons[2]);
        StmtAnalyze(sons[4]);
        break;
    default:
        break;
    }
}

int ExtDecListAnalyze(int index, Type *type)
{
    ASTNode ext_dec_list = nodes[index];
    DebugPrintNameType(ext_dec_list);
    int *sons = GetSon(ext_dec_list);
    switch (ext_dec_list.type)
    {
    case ExtDecList_VarDec:
        VarDecAnalyze(sons[0], type, NULL, VAR);
        break;
    case ExtDecList_VarDecCOMMAExtDecList:
    {
        VarDecAnalyze(sons[0], type, NULL, VAR);
        ExtDecListAnalyze(sons[2], type);
        break;
    }
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
            SemanticError(16, struct_.lineno, "Duplicated name", name);
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
            SemanticError(17, struct_.lineno, "using structure to def var before declare", "");
        }
        else
        {
            //TODO need add other var into symbol_table
            Symbol *symbol = SymbolGet(name, STRUCT);
            // type->kind = symbol->type->kind;
            // type->value = symbol->type->value;
            // type->field = symbol->type->field;
            *type = *symbol->type;
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
    DebugPrintNameType(def_list);
    if (def_list.type == DefList_DefDefList)
    {
        int *sons = GetSon(def_list);
        Field *field_ = malloc(sizeof(Field));
        DefAnalyze(sons[0], field, kind);
        int ret = DefListAnalyze(sons[1], field_, kind);
        if (kind == STRUCT)
        {
            if (ret == -1)
            {
                free(field_);
                return 0;
            }
            if (ret == 0)
            {
                while (field->next != NULL)
                    field = field->next;
                field->next = field_;
            }
        }
    }
    else
        return -1;
    return 0;
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
        DecListAnalyze(sons[1], type, NULL, kind);
        break;
    case STRUCT:
        DecListAnalyze(sons[1], type, field, kind);
        break;
    default:
        break;
    }
    // printf("%d\n", type->kind);
}

int DecListAnalyze(int index, Type *type, Field *field, SymbolKind kind)
{
    ASTNode dec_list = nodes[index];
    DebugPrintNameType(dec_list);
    int *sons = GetSon(dec_list);
    // DecAnalyze(sons[0], type, kind);
    switch (dec_list.type)
    {
    case DecList_Dec:
        DecAnalyze(sons[0], type, field, kind);
        break;
    case DecList_DecCOMMADecList:
    {
        if (kind == STRUCT)
        {
            DecAnalyze(sons[0], type, field, kind);
            Field *field_ = malloc(sizeof(Field));
            DecListAnalyze(sons[2], type, field_, kind);
            field->next = field_;
            break;
        }
        if (kind == VAR)
        {
            DecAnalyze(sons[0], type, NULL, kind);
            DecListAnalyze(sons[2], type, NULL, kind);
        }
    }
    default:
        break;
    }
}

int DecAnalyze(int index, Type *type, Field *field, SymbolKind kind)
{
    ASTNode dec = nodes[index];
    DebugPrintNameType(dec);
    int *sons = GetSon(dec);
    // VarDecAnalyze(dec.child, type, kind);
    switch (dec.type)
    {
    case Dec_VarDec:
        VarDecAnalyze(sons[0], type, field, kind);
        break;
    case Dec_VarDecASSIGNOPExp:
    {
        ASTNode var_dec = nodes[sons[0]];
        char *name = nodes[var_dec.child].value;
        if (kind == STRUCT)
        {
            SemanticError(15, dec.lineno, "Initializes on the field of the structure about", name);
        }
        //TODO ADD VALUE HERE
        VarDecAnalyze(sons[0], type, field, kind);
        break;
    }
    default:
        break;
    }
}

int VarDecAnalyze(int index, Type *type, Field *field, SymbolKind kind)
{
    ASTNode var_dec = nodes[index];
    DebugPrintNameType(var_dec);
    int *sons = GetSon(var_dec);
    switch (var_dec.type)
    {
    case VarDec_ID:
    {
        char *name = nodes[sons[0]].value;
        if (SymbolContains(name, kind) == 1)
        {
            if (kind == VAR)
                SemanticError(3, var_dec.lineno, "Redefine Variable", name);
            if (kind == STRUCT)
                {
                    SemanticError(15, var_dec.lineno, "Redefine Field", name);
                    field->name = random();
                    field->type = malloc(sizeof(Type));
                    *field->type = *type;
                    field->next= NULL;
                }
        }
        else
        {
            Symbol *symbol = malloc(sizeof(Symbol));
            symbol->name = name;
            symbol->next = NULL;
            if (kind == VAR)
            {
                symbol->name = name;
                symbol->kind = VAR;
                symbol->type = type;
                symbol->next = NULL;
            }
            if (kind == STRUCT)
            {
                field->type = malloc(sizeof(Type));
                *field->type = *type;
                field->name = name;
                field->next = NULL;
                symbol->kind = FIELD;
                symbol->name = name;
            }
            SymbolInsert(symbol);
        }
        break;
    }
    case VarDec_VarDecLbIntRb:
    {
        //TODO ADD HERE
        //int a[10][2] -> a array(array(int,2),10)
        Type* type_ = malloc(sizeof(Type));
        type_->value = LEFT;
        type_->kind = ARRAY;
        type_->array = malloc(sizeof(type_->array));
        type_->array->type = malloc(sizeof(Type));
        *type_->array->type = *type;
        type_->array->size = IntAnalyze(sons[2]);
        VarDecAnalyze(sons[0], type_, field, kind);
        break;
    }
    default:
        break;
    }
}

Type *ExpAnalyze(int index)
{
    ASTNode exp = nodes[index];
    DebugPrintNameType(exp);
    int *sons = GetSon(exp);
    switch (exp.type)
    {
    case Exp_ExpAssignopExp:
    {
        Type *t1 = ExpAnalyze(sons[0]);
        Type *t2 = ExpAnalyze(sons[2]);
        if (t1 != NULL && t1->value == LEFT)
        {
            SemanticError(6, exp.lineno, "The left-hand side of an assignment must be a variable.", "");
        }
        if (t1 != NULL && t2 != NULL && TypeKindEqual(t1, t2) != 1)
        {
            SemanticError(5, exp.lineno, " Type mismatched for assignment.", "");
        }
        return t1;
    }
    case Exp_ExpAndExp:
    case Exp_ExpOrExp:
    case Exp_ExpRelopExp:
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

int ArgsAnalyze(int index)
{
    ASTNode args = nodes[index];
    DebugPrintNameType(args);
}

int IntAnalyze(int index)
{
    ASTNode Int = nodes[index];
    return 3;
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