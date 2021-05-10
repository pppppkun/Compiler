#include "semantic.h"

void DebugPrintNameType(ASTNode *node)
{
    if (IR_DEBUG)
    {
        printf("%s %d %d\n", node->name, node->type, node->lineno);
    }
}

void DebugPrintNameValue(ASTNode *node)
{
    if (IR_DEBUG)
    {
        printf("%s %s\n", node->name, node->value);
    }
}

int *GetSon(ASTNode *node)
{
    int *sons = malloc(sizeof(int) * 10);
    int index = 0;
    sons[index++] = node->child;
    ASTNode *child = nodes[sons[index - 1]];
    while (child->brother != -1)
    {
        sons[index++] = child->brother;
        child = nodes[sons[index - 1]];
    }
    return sons;
}

void DebugPrintSymbol()
{
    if (!IR_DEBUG)
        return;
    for (int i = 0; i < nodes_point; i++)
    {
        Symbol *symbol = symbol_table[i];
        if (symbol->lineno == -1)
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
                case FUNCTION:
                    kind = "function";
                    break;
                case FUNCTION_PARAM:
                    kind = "function param";
                    break;
                default:
                    break;
                }
                printf("%s %s\n", kind, symbol->name);
                if (symbol->kind == STRUCT || (symbol->kind == VAR && symbol->type->kind == STRUCTURE))
                {
                    Field *field = symbol->type->field;
                    while (field != NULL)
                    {
                        char *field_kind;
                        switch (field->type->kind)
                        {
                        case BASIC:
                            if (field->type->basic == INT)
                                field_kind = "int";
                            else
                                field_kind = "float";
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
                        printf("\t%s %s %s.%s\n", "field", field_kind, symbol->name, field->name);
                        if (field->type->kind == ARRAY)
                        {
                            Type *type = field->type;
                            while (type->kind == ARRAY)
                            {
                                printf("\t\tarray size is %d\n", type->array->size);
                                type = type->array->type;
                            }
                            printf("\t\tfinal type is %d\n", type->kind);
                        }
                        field = field->next;
                    }
                }
                if (symbol->kind == VAR && symbol->type->kind == ARRAY)
                {
                    Type *type = symbol->type;
                    while (type->kind == ARRAY)
                    {
                        printf("\tarray size is %d\n", type->array->size);
                        type = type->array->type;
                    }
                    printf("\tfinal type is %d\n", type->kind);
                }
                if (symbol->kind == FUNCTION)
                {
                    Field *field_ = symbol->type->field;
                    while (field_ != NULL)
                    {
                        printf("%s ", field_->name);
                        field_ = field_->next;
                    }
                    printf("\n");
                }
                symbol = symbol->next;
            }
        }
    }
}

void SemanticError(int typeno, int lineno, char *reason, char *addition)
{
    printf("Error type %d at Line %d: %s ", typeno, lineno, reason);
    if (addition != NULL && strlen(addition) != 0)
    {
        printf("about %s.\n", addition);
    }
    else
        printf(".\n");
    //Error type 1 at Line 4: Undefined variable "j".
}

char *random()
{
    char *name = malloc(sizeof(char) * 10);
    for (int i = 0; i < 10; i++)
    {
        int t = rand() % (10 + 26 + 26);
        if (t < 10)
            name[i] = (char)('0' + t);
        else if (t < 10 + 26)
            name[i] = (char)('a' + t - 10);
        else
            name[i] = (char)('A' + t - 10 - 26);
    }
    return name;
}

int TypeEqual(Type *t1, Type *t2)
{
    if (t1 != NULL && t2 != NULL)
    {
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
            return TypeEqual(t1->array->type, t2->array->type);
        case STRUCTURE:
            if (strcmp(t1->struct_name, t2->struct_name) == 0)
                return 1;
            else
                return 0;
        default:
            break;
        }
    }
    else
        return 0;
    //TODO
    // if(t1!=NULL && t2!=NULL)
    // {
    //     printf("t1 and t2 all not null\n");
    // }
    // else
    // {
    //     printf("Something wrong\n");
    //     if(t1 == NULL) printf("t1 is null\n");
    //     if(t2 == NULL) {
    //         printf("%d\n",t1->kind);
    //     }
    // }
}

int TypeBasicJudge(Type *t1, BasicType basic)
{
    if (t1->kind == BASIC && t1->basic == basic)
        return 1;
    else
        return 0;
}

int TypeKindJudge(Type *t1, TypeKind kind)
{
    if (t1->kind == kind)
        return 1;
    else
        return 0;
}

int FuncParamEqual(Field *f1, Field *f2)
{
    while (f1 != NULL && f2 != NULL)
    {
        // printf("%d %d\n", f1->type->kind, f2->type->kind);
        if (TypeEqual(f1->type, f2->type) == 1)
        {
            f1 = f1->next;
            f2 = f2->next;
        }
        else
            return 0;
    }
    if (f1 == NULL && f2 == NULL)
        return 1;
    else
        return 0;
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

int SymbolInsert(Symbol *symbol)
{
    char *name = symbol->name;
    unsigned int index = hash_pjw(name);
    // first insert
    if (symbol_table[index]->lineno == -1)
    {
        free(symbol_table[index]);
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
}
// if contains a symbol which type is kind and name is name will return 1.
// else return 0.
int SymbolContains(char *name, SymbolKind kind)
{
    unsigned int index = hash_pjw(name);
    Symbol *symbol = symbol_table[index];
    if (symbol->lineno == -1)
        return 0;
    while (symbol != NULL)
    {
        if (strcmp(name, symbol->name) == 0)
        {
            // printf("%s %d\n", symbol->name, symbol->kind);
            switch (symbol->kind)
            {
            case VAR:
                if (kind == VAR || kind == FIELD || kind == FUNCTION_PARAM || kind == STRUCT)
                    return 1;
                break;
            case FIELD:
                if (kind == FIELD || kind == VAR || kind == FUNCTION_PARAM || kind == STRUCT)
                    return 1;
                break;
            case STRUCT:
                if (kind == VAR || kind == STRUCT || kind == FIELD || kind == FUNCTION_PARAM)
                    return 1;
                break;
            case FUNCTION_PARAM:
                printf("%s\n", symbol->name);
                if (kind == VAR || kind == STRUCT || kind == FIELD || kind == FUNCTION_PARAM)
                    return 1;
                break;
            case FUNCTION:
                if (kind == FUNCTION)
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
//given function is judge whether or not.
//1. 多次声明是冲突的.
//2. 声明和定义是冲突的.
// 0 表示没有冲突 1表示冲突
int UndefFunctionJudge(Symbol *symbol, int lineno)
{
    char *name = symbol->name;
    //TODO 这里会有一个链条
    unsigned int index = hash_pjw(name);
    Symbol *symbol_ = symbol_table[index];
    if (symbol_->lineno == -1)
        return 0;
    while (symbol_ != NULL)
    {
        if (strcmp(name, symbol_->name) != 0)
        {
            symbol_ = symbol_->next;
            continue;
        }
        if (symbol->kind == UNDEF)
        {
            if (symbol_->kind == DEF || symbol_->kind == UNDEF)
            {
                //TODO
                Field *f1 = symbol->type->field;
                Field *f2 = symbol_->type->field;
                if (FuncParamEqual(f1, f2) == 0)
                {
                    SemanticError(19, lineno, "Inconsistent declaration of function", name);
                    return 1;
                }
            }
        }
        if (symbol->kind == DEF)
        {
            if (symbol_->kind == UNDEF)
            {
                Field *f1 = symbol->type->field;
                Field *f2 = symbol_->type->field;
                if (FuncParamEqual(f1, f2) == 0)
                {
                    SemanticError(19, lineno, "Inconsistent declaration of function", name);
                    return 1;
                }
            }
        }
        symbol_ = symbol_->next;
    }
    return 0;
}
//-1 error
//0 undef
//1 def
int checkWhetherDef(Symbol *symbol, char *name)
{
    if (symbol == NULL || symbol->lineno == -1)
        return -1;
    while (symbol != NULL)
    {
        if (symbol->kind == FUNCTION && (strcmp(symbol->name, name) == 0) && symbol->type->kind == DEF)
            return 1;
        else
            symbol = symbol->next;
    }
    return 0;
}

int ProgramAnalyze(int index)
{
    ASTNode *program = nodes[index];
    DebugPrintNameType(program);
    ASTNode *child = nodes[program->child];
    // printf("%s %s %s\n", child.name, child.value, node.name);
    ExtDefListAnalyze(program->child);
    //DebugPrintSymbol();
    return 0;
}

int ExtDefListAnalyze(int index)
{
    ASTNode *ext_def_list = nodes[index];
    //type = 1 represent ExtDefList -> ExtDef ExtDefList
    if (ext_def_list->type == ExtDefList_ExtDefExtDefList)
    {
        int ext_def = ext_def_list->child;
        ExtDefAnalyze(ext_def);
        int ext_def_list = nodes[ext_def]->brother;
        ExtDefListAnalyze(ext_def_list);
    }
    else
        return -1;
    return 0;
}

int ExtDefAnalyze(int index)
{
    ASTNode *ext_def_node = nodes[index];
    DebugPrintNameType(ext_def_node);
    // int child = ext_def_node.child;
    int *sons = GetSon(ext_def_node);
    ASTNode *child_node = nodes[sons[0]];
    Type *type = malloc(sizeof(Type));
    switch (ext_def_node->type)
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
        FunDecAnalyze(sons[1], type, UNDEF);
        break;
    case ExtDef_SpecifierFunDecCompSt:
        SpecifierAnalyze(sons[0], type);
        type->value = RIGHT;
        FunDecAnalyze(sons[1], type, DEF);
        char *name = nodes[nodes[sons[1]]->child]->value;
        Symbol *func = SymbolGet(name, FUNCTION);
        CompStAnalyze(sons[2], func);
        break;
    default:
        break;
    }
}

int FunDecAnalyze(int index, Type *type, TypeKind kind)
{
    ASTNode *fun_dec = nodes[index];
    DebugPrintNameType(fun_dec);
    int *sons = GetSon(fun_dec);
    char *name = nodes[sons[0]]->value;
    if (SymbolContains(name, FUNCTION) == 1)
    {
        //TODO
        Symbol *func = SymbolGet(name, FUNCTION);
        if (func->type->kind == DEF && kind == DEF)
        {
            SemanticError(4, fun_dec->lineno, "Redefined function", name);
            return 0;
        }
    }
    Symbol *symbol = malloc(sizeof(Symbol));
    symbol->kind = FUNCTION;
    symbol->name = name;
    symbol->next = NULL;
    symbol->type = malloc(sizeof(Type));
    symbol->type->kind = kind;
    symbol->lineno = fun_dec->lineno;
    switch (fun_dec->type)
    {
    case FunDec_IDLPVarListRP:
    {
        Field *field = VarListAnalyze(sons[2], kind);
        Field *field_ = malloc(sizeof(Field));
        field_->name = "RETURN";
        field_->type = type;
        field_->next = field;
        symbol->type->field = malloc(sizeof(Field));
        *symbol->type->field = *field_;
        break;
    }
    case FunDec_IDLPRP:
    {
        Field *field_ = malloc(sizeof(Field));
        field_->name = "RETURN";
        field_->type = type;
        symbol->type->field = malloc(sizeof(Field));
        *symbol->type->field = *field_;
        break;
    }
    default:
        break;
    }
    if (UndefFunctionJudge(symbol, fun_dec->lineno) == 0)
    {
        SymbolInsert(symbol);
    }
}

Field *VarListAnalyze(int index, TypeKind func_type)
{
    ASTNode *var_list = nodes[index];
    DebugPrintNameType(var_list);
    int *sons = GetSon(var_list);
    switch (var_list->type)
    {
    case VarList_ParamDecCOMMAVarList:
    {
        Field *field = ParamDecAnalyze(sons[0], func_type);
        Field *field_ = VarListAnalyze(sons[2], func_type);
        field->next = field_;
        return field;
    }
    case VarList_ParamDec:
        return ParamDecAnalyze(sons[0], func_type);
    default:
        break;
    }
}

int UndefFuncParamAnalyze(int index, Type *type, Field *field)
{
    ASTNode *param = nodes[index];
    DebugPrintNameType(param);
    // printf("actually enter undef func param analyze\n");
    int *sons = GetSon(param);
    switch (param->type)
    {
    case VarDec_ID:
    {
        field->type = malloc(sizeof(Type));
        *field->type = *type;
        field->next = NULL;
        break;
    }
    case VarDec_VarDecLbIntRb:
    {
        Type *type_ = malloc(sizeof(Type));
        type_->value = LEFT;
        type_->kind = ARRAY;
        type_->array = malloc(sizeof(type_->array));
        type_->array->type = malloc(sizeof(Type));
        *type_->array->type = *type;
        type_->array->size = IntAnalyze(sons[2]);
        UndefFuncParamAnalyze(sons[0], type_, field);
        break;
    }
    default:
        break;
    }
}

Field *ParamDecAnalyze(int index, TypeKind func_type)
{
    ASTNode *param_dec = nodes[index];
    DebugPrintNameType(param_dec);
    int *sons = GetSon(param_dec);
    Type *type = malloc(sizeof(Type));
    SpecifierAnalyze(sons[0], type);
    // printf("%s\n", type->struct_name);
    Field *field = malloc(sizeof(field));
    if (func_type == DEF)
        VarDecAnalyze(sons[1], type, field, FUNCTION_PARAM);
    else
        UndefFuncParamAnalyze(sons[1], type, field);
    return field;
}

int CompStAnalyze(int index, Symbol *func)
{
    ASTNode *comp_st = nodes[index];
    DebugPrintNameType(comp_st);
    int *sons = GetSon(comp_st);
    DefListAnalyze(sons[1], NULL, VAR);
    StmtListAnalyze(sons[2], func);
}

int StmtListAnalyze(int index, Symbol *func)
{
    ASTNode *stmt_list = nodes[index];
    DebugPrintNameType(stmt_list);
    if (stmt_list->type == StmtList_StmtStmtList)
    {
        int *sons = GetSon(stmt_list);
        StmtAnalyze(sons[0], func);
        StmtListAnalyze(sons[1], func);
    }
}

int StmtAnalyze(int index, Symbol *func)
{
    ASTNode *stmt = nodes[index];
    int *sons = GetSon(stmt);
    DebugPrintNameType(stmt);
    switch (stmt->type)
    {
    case Stmt_ExpSEMI:
        ExpAnalyze(sons[0]);
        break;
    case Stmt_CompSt:
        CompStAnalyze(sons[0], func);
        break;
    case Stmt_ReturnExpSEMI:
    {
        Type *type = ExpAnalyze(sons[1]);
        Type *ret = func->type->field->type;
        if (type != NULL && ret != NULL && TypeEqual(type, ret) != 1)
        {
            SemanticError(8, stmt->lineno, "Type mismatched for return", NULL);
        }
        break;
    }
    case Stmt_IfLpExpRpStmt:
        ExpAnalyze(sons[2]);
        StmtAnalyze(sons[4], func);
        break;
    case Stmt_IfLpExpRpStmtElseStmt:
        ExpAnalyze(sons[2]);
        StmtAnalyze(sons[4], func);
        StmtAnalyze(sons[6], func);
        break;
    case Stmt_WhileLpExpRpStmt:
        ExpAnalyze(sons[2]);
        StmtAnalyze(sons[4], func);
        break;
    default:
        break;
    }
}

int ExtDecListAnalyze(int index, Type *type)
{
    ASTNode *ext_dec_list = nodes[index];
    DebugPrintNameType(ext_dec_list);
    int *sons = GetSon(ext_dec_list);
    switch (ext_dec_list->type)
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
    ASTNode *specifier = nodes[index];
    DebugPrintNameType(specifier);
    ASTNode *spe_child = nodes[specifier->child];
    switch (specifier->type)
    {
    case Specifier_TYPE:
    {
        type->kind = BASIC;
        type->value = LEFT;
        int res = strcmp(spe_child->value, "int");
        if (res == 0)
            type->basic = INT;
        else
            type->basic = FLOAT;
        return 1;
    }
    case Specifier_StructSpecifier:
    {
        StructAnalyze(specifier->child, type);
        break;
    }
    default:
        break;
    }
}

int StructAnalyze(int index, Type *type)
{
    ASTNode *struct_ = nodes[index];
    DebugPrintNameType(struct_);
    int *sons = GetSon(struct_);
    switch (struct_->type)
    {
    case StructSpecifier_STRUCTOptTagLCDefListRC:
    {
        char *name = OptTagAnalyze(sons[1]);
        type->kind = STRUCTURE;
        type->value = LEFT;
        if (SymbolContains(name, STRUCT))
        {
            SemanticError(16, struct_->lineno, "Duplicated name", name);
        }
        else
        {
            Symbol *symbol = malloc(sizeof(Symbol));
            symbol->name = name;
            symbol->kind = STRUCT;
            symbol->type = type;
            symbol->next = NULL;
            symbol->lineno = struct_->lineno;
            type->struct_name = name;
            Field *field = malloc(sizeof(Field));
            type->field = field;
            DefListAnalyze(sons[3], field, FIELD);
            SymbolInsert(symbol);
        }
        break;
    }
    case StructSpecifier_STRUCTTag:
    {
        char *name = TagAnalyze(sons[1]);
        if (SymbolContains(name, STRUCT) == 0)
        {
            SemanticError(17, struct_->lineno, "using structure to def var before declare", NULL);
        }
        else
        {
            Symbol *symbol = SymbolGet(name, STRUCT);
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
    ASTNode *def_list = nodes[index];
    DebugPrintNameType(def_list);
    if (def_list->type == DefList_DefDefList)
    {
        int *sons = GetSon(def_list);
        Field *field_ = malloc(sizeof(Field));
        DefAnalyze(sons[0], field, kind);
        int ret = DefListAnalyze(sons[1], field_, kind);
        if (kind == FIELD)
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
    ASTNode *def = nodes[index];
    int *sons = GetSon(def);
    DebugPrintNameType(def);
    Type *type = malloc(sizeof(type));
    SpecifierAnalyze(sons[0], type);
    switch (kind)
    {
    case VAR:
        DecListAnalyze(sons[1], type, NULL, kind);
        break;
    case FIELD:
        DecListAnalyze(sons[1], type, field, kind);
        break;
    default:
        break;
    }
    // printf("%d\n", type->kind);
}

int DecListAnalyze(int index, Type *type, Field *field, SymbolKind kind)
{
    ASTNode *dec_list = nodes[index];
    DebugPrintNameType(dec_list);
    int *sons = GetSon(dec_list);
    // DecAnalyze(sons[0], type, kind);
    switch (dec_list->type)
    {
    case DecList_Dec:
        DecAnalyze(sons[0], type, field, kind);
        break;
    case DecList_DecCOMMADecList:
    {
        if (kind == FIELD)
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
    ASTNode *dec = nodes[index];
    DebugPrintNameType(dec);
    int *sons = GetSon(dec);
    // VarDecAnalyze(dec.child, type, kind);
    switch (dec->type)
    {
    case Dec_VarDec:
        VarDecAnalyze(sons[0], type, field, kind);
        break;
    case Dec_VarDecASSIGNOPExp:
    {
        ASTNode *var_dec = nodes[sons[0]];
        char *name = nodes[var_dec->child]->value;
        //TODO ADD VALUE HERE
        VarDecAnalyze(sons[0], type, field, kind);
        if (kind == FIELD)
        {
            SemanticError(15, dec->lineno, "Initializes on the field of the structure about", name);
        }
        if (kind == VAR)
        {
            if (type->value == RIGHT)
            {
                SemanticError(6, dec->lineno, "The left-hand side of an assignment must be a variable", NULL);
            }
            Type *t1 = ExpAnalyze(sons[2]);
            if (TypeEqual(t1, type) == 0)
            {
                SemanticError(5, dec->lineno, "Type mismatched for assignment", NULL);
            }
        }
        break;
    }
    default:
        break;
    }
}

int VarDecAnalyze(int index, Type *type, Field *field, SymbolKind kind)
{
    ASTNode *var_dec = nodes[index];
    DebugPrintNameType(var_dec);
    int *sons = GetSon(var_dec);
    switch (var_dec->type)
    {
    case VarDec_ID:
    {
        char *name = nodes[sons[0]]->value;
        if (SymbolContains(name, kind) == 1)
        {
            if (kind == VAR)
                SemanticError(3, var_dec->lineno, "Redefine Variable", name);
            if (kind == FIELD)
            {
                SemanticError(15, var_dec->lineno, "Redefine Field", name);
                field->name = random();
                field->type = malloc(sizeof(Type));
                *field->type = *type;
                field->next = NULL;
            }
            if (kind == FUNCTION_PARAM)
            {
                SemanticError(3, var_dec->lineno, "Redefine function param", name);
                field->name = random();
                field->type = malloc(sizeof(Type));
                *field->type = *type;
                field->next = NULL;
            }
        }
        else
        {
            Symbol *symbol = malloc(sizeof(Symbol));
            symbol->name = name;
            symbol->next = NULL;
            symbol->lineno = var_dec->lineno;
            if (kind == VAR)
            {
                symbol->name = name;
                symbol->kind = kind;
                symbol->type = type;
                symbol->next = NULL;
            }
            if (kind == FIELD)
            {
                field->type = malloc(sizeof(Type));
                *field->type = *type;
                field->name = name;
                field->next = NULL;
                symbol->kind = kind;
                symbol->name = name;
            }
            if (kind == FUNCTION_PARAM)
            {
                field->type = malloc(sizeof(Type));
                *field->type = *type;
                field->name = name;
                field->next = NULL;
                symbol->kind = VAR;
                symbol->name = name;
                symbol->type = type;
            }
            SymbolInsert(symbol);
        }
        break;
    }
    case VarDec_VarDecLbIntRb:
    {
        Type *type_ = malloc(sizeof(Type));
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
    ASTNode *exp = nodes[index];
    DebugPrintNameType(exp);
    int *sons = GetSon(exp);
    switch (exp->type)
    {
    case Exp_ExpAssignopExp:
    {
        Type *t1 = ExpAnalyze(sons[0]);
        Type *t2 = ExpAnalyze(sons[2]);
        if (t1 != NULL && t1->value == RIGHT)
        {
            SemanticError(6, exp->lineno, "The left-hand side of an assignment must be a variable", NULL);
        }
        if (t1 != NULL && t2 != NULL && TypeEqual(t1, t2) != 1)
        {
            SemanticError(5, exp->lineno, "Type mismatched for assignment", NULL);
        }
        if (t1 != NULL)
            return t1;
        else
            return t2;
    }
    case Exp_ExpAndExp:
    case Exp_ExpOrExp:
    case Exp_ExpRelopExp:
    {
        Type *t1 = ExpAnalyze(sons[0]);
        Type *t2 = ExpAnalyze(sons[2]);
        if (t1 != NULL && TypeBasicJudge(t1, INT) == 0)
            SemanticError(7, exp->lineno, "Type mismatched for operands", "Boolean Exp only support INT");
        if (t2 != NULL && TypeBasicJudge(t2, INT) == 0)
            SemanticError(7, exp->lineno, "Type mismatched for operands", "Boolean Exp only support INT");
        Type *type = malloc(sizeof(Type));
        type->kind = BASIC;
        type->value = RIGHT;
        type->basic = INT;
        return type;
    }
    case Exp_ExpPlusExp:
    case Exp_ExpMinusExp:
    case Exp_ExpStarExp:
    case Exp_ExpDivExp:
    {
        Type *t1 = ExpAnalyze(sons[0]);
        Type *t2 = ExpAnalyze(sons[2]);
        if (t1 != NULL && t2 != NULL)
        {
            if (TypeKindJudge(t1, BASIC) == 0 || TypeKindJudge(t2, BASIC) == 0)
                SemanticError(7, exp->lineno, "Type mismatched for operands", "Arithmetic Exp only support Basic Type (INT or FLOAT)");
            if (TypeEqual(t1, t2) == 0)
                SemanticError(7, exp->lineno, "Type mismatched for operands", "The types on both sides of an arithmetic expression need to be the same");
        }
        if (t1 != NULL)
            return t1;
        else
            return t2;
    }
    case Exp_LpExpRp:
        return ExpAnalyze(sons[1]);
    case Exp_MinusExp:
    {
        Type *t1 = ExpAnalyze(sons[1]);
        if (t1 != NULL && TypeKindJudge(t1, BASIC) == 0)
            SemanticError(7, exp->lineno, "Type mismatched for operands", "Arithmetic Exp only support Basic Type (INT or FLOAT)");
        return t1;
    }
    case Exp_NotExp:
    {
        Type *t1 = ExpAnalyze(sons[1]);
        if (t1 != NULL && TypeBasicJudge(t1, INT) == 0)
            SemanticError(7, exp->lineno, "Type mismatched for operands", "Boolean Exp only support INT");
        Type *type = malloc(sizeof(Type));
        type->kind = BASIC;
        type->basic = INT;
        type->value = RIGHT;
        return type;
    }
    case Exp_IdLpArgsRp:
    {
        char *name = nodes[sons[0]]->value;
        Field *func_params = ArgsAnalyze(sons[2]);
        if (SymbolContains(name, VAR) == 1)
        {
            SemanticError(11, exp->lineno, "Not a function", name);
            return NULL;
        }
        if (SymbolContains(name, FUNCTION) == 0 && SymbolContains(name, VAR) == 0)
        {
            SemanticError(2, exp->lineno, "Undefined function", name);
            return NULL;
        }
        Symbol *func = SymbolGet(name, FUNCTION);
        if (FuncParamEqual(func->type->field->next, func_params) == 0)
        {
            SemanticError(9, exp->lineno, "The number or type of the actual participating parameters does not match during a function call", name);
            return NULL;
        }
        if (SymbolGet(name, FUNCTION)->type->kind == UNDEF)
        {
            SemanticError(18, exp->lineno, "This function is declare but not defined", name);
            return NULL;
        }
        //TODO need fix type
        return func->type->field->type;
    }
    case Exp_IdLpRp:
    {
        char *name = nodes[sons[0]]->value;
        if (SymbolContains(name, VAR) == 1)
        {
            SemanticError(11, exp->lineno, "Not a function", name);
            return NULL;
        }
        if (SymbolContains(name, FUNCTION) == 0)
        {
            SemanticError(2, exp->lineno, "Undefined function", name);
            return NULL;
        }
        Symbol *func = SymbolGet(name, FUNCTION);
        if (func->type->field->next != NULL)
        {
            SemanticError(9, exp->lineno, "The number or type of the actual participating parameters does not match during a function call", name);
            return NULL;
        }
        if (SymbolGet(name, FUNCTION)->type->kind == UNDEF)
        {
            SemanticError(18, exp->lineno, "This function is declare but not defined", name);
            return NULL;
        }
        // if(func->type->field==NULL)
        //     return NULL;
        // else return
        return func->type->field->type;
    }
    case Exp_ExpLbExpRb:
    {
        Type *t1 = ExpAnalyze(sons[0]);
        Type *t2 = ExpAnalyze(sons[2]);
        if (t1 != NULL && t1->kind != ARRAY)
        {
            SemanticError(10, exp->lineno, "Not a array", NULL);
            return NULL;
        }
        if (t2 != NULL && TypeBasicJudge(t2, INT) == 0)
        {
            SemanticError(12, exp->lineno, "Only integers can be used as array subscripts", NULL);
            return NULL;
        }
        if (t1 == NULL)
            return NULL;
        else
            return t1->array->type;
    }
    case Exp_ExpDotId:
    {
        Type *t1 = ExpAnalyze(sons[0]);
        if (t1 == NULL)
        {
            return NULL;
        }
        if (t1->kind != STRUCTURE)
        {
            SemanticError(13, exp->lineno, "using dot on non-struct var", NULL);
            return NULL;
        }
        char *name = nodes[sons[2]]->value;
        Field *field = t1->field;
        while (field != NULL)
        {
            if (strcmp(field->name, name) == 0)
                return field->type;
            else
                field = field->next;
        }
        SemanticError(14, exp->lineno, "Non-existent field", name);
        return NULL;
    }
    case Exp_Id:
    {
        char *name = nodes[sons[0]]->value;
        if (SymbolContains(name, VAR) == 0)
        {
            SemanticError(1, exp->lineno, "Undefined variable", name);
            return NULL;
        }

        Symbol *s1 = SymbolGet(name, VAR);
        // printf("%s %d\n", s1->name, s1->type->kind);
        if (s1 == NULL)
        {
            return NULL;
        }
        else
            return s1->type;
    }
    case Exp_Int:
    case Exp_Float:
    {
        Type *t1 = malloc(sizeof(Type));
        t1->kind = BASIC;
        t1->value = RIGHT;
        if (exp->type == Exp_Int)
            t1->basic = INT;
        else
            t1->basic = FLOAT;
        return t1;
    }
    default:
        break;
    }
}

char *OptTagAnalyze(int index)
{
    ASTNode *opt_tag = nodes[index];
    switch (opt_tag->type)
    {
    case OptTag_ID:
        return nodes[opt_tag->child]->value;
    default:
        return random();
    }
}

char *TagAnalyze(int index)
{
    ASTNode *tag = nodes[index];
    return nodes[tag->child]->value;
}

Field *ArgsAnalyze(int index)
{
    ASTNode *args = nodes[index];
    int *sons = GetSon(args);
    DebugPrintNameType(args);
    switch (args->type)
    {
    case Args_ExpCommaArgs:
    {
        Type *t1 = ExpAnalyze(sons[0]);
        Field *f2 = malloc(sizeof(Field));
        f2->type = t1;
        Field *f1 = ArgsAnalyze(sons[2]);
        f2->next = f1;
        return f2;
    }
    case Args_Exp:
    {
        Field *f1 = malloc(sizeof(Field));
        f1->type = ExpAnalyze(sons[0]);
        f1->next = NULL;
        return f1;
    }
    default:
        break;
    }
}
int IntAnalyze(int index)
{
    ASTNode *Int = nodes[index];
    char *value = Int->value;
    int len = strlen(value);
    int ret = 0;
    for (int i = 0; i < len; i++)
    {
        ret = ret + value[i] - '0';
        ret = ret * 10;
    }
    return ret / 10;
}

int semanticAnalyze(int last_node)
{
    symbol_table = malloc(sizeof(Symbol *) * nodes_point);
    for (int i = 0; i < nodes_point; i++)
    {
        symbol_table[i] = malloc(sizeof(Symbol *) * nodes_point);
        symbol_table[i]->lineno = -1;
    }
    ProgramAnalyze(last_node);
    for (int i = 0; i < nodes_point; i++)
    {
        if (symbol_table[i]->lineno == -1)
            continue;
        else
        {
            Symbol *symbol = symbol_table[i];
            while (symbol != NULL)
            {
                if (symbol->kind == FUNCTION && symbol->type->kind == UNDEF)
                {
                    if (checkWhetherDef(symbol_table[i], symbol->name) == 0)
                        SemanticError(18, symbol->lineno, "Undefined function", symbol->name);
                }
                symbol = symbol->next;
            }
        }
    }
}