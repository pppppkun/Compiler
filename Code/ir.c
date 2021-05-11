#include "ir.h"

int v_index = 1;
int label_index = 1;

void insert_code(InterCode *code)
{
    now->code = malloc(sizeof(InterCode));
    *(now->code) = *code;
    now->next = malloc(sizeof(InterCodes));
    now->next->prev = now;
    now = now->next;
    now->next = NULL;
    if (IR_DEBUG)
    {
        printf("%d\n", now->prev->code->kind);
        printf("Insert Code Success\n");
    }
}

void print_var_or_constant(Operand *o)
{
    if (o->kind == CONSTANT)
        printf("#%d", o->u.value);
    else
        printf("v%d", o->u.var_no);
}

void print_binop(InterCode *code, char binop)
{
    Operand *left = code->u.binop.op1;
    Operand *right = code->u.binop.op2;
    Operand *result = code->u.binop.result;
    printf("v%d := ", result->u.var_no);
    print_var_or_constant(left);
    printf(" %c ", binop);
    print_var_or_constant(right);
    printf("\n");
}

void print_arg(Variable *args)
{
    while (args != NULL && args->operand != NULL)
    {
        printf("ARG v%d\n", args->operand->u.var_no);
        args = args->prev;
    }
}

void print_param(Variable *param)
{
    while (param != NULL && param->operand != NULL)
    {
        printf("PARAM v%d\n", param->operand->u.var_no);
        param = param->next;
    }
}

void print_ir()
{
    InterCodes *head = codes->next;
    while (head != NULL && head->code != NULL)
    {
        InterCode *code = head->code;
        switch (code->kind)
        {
        case LABEL:
            printf("LABEL label%d :\n", label_index);
            break;
        case IR_FUNCTION:
            printf("FUNCTION %s :\n", head->code->u.function_name);
            break;
        case ASSIGN:
        {
            Operand *left = code->u.assign.left;
            Operand *right = code->u.assign.right;
            printf("v%d := ", left->u.var_no);
            print_var_or_constant(right);
            printf("\n");
            break;
        }
        case ADD:
            print_binop(code, '+');
            break;
        case SUB:
            print_binop(code, '-');
            break;
        case MUL:
            print_binop(code, '*');
            break;
        case DIV:
            print_binop(code, '/');
            break;
        case GOTO:
            printf("GOTO label%d\n", code->u.label_index);
        case IF_GOTO:
            printf("IF ");
            print_var_or_constant(code->u.if_go.op1);
            printf(" %s ", code->u.if_go.relop);
            print_var_or_constant(code->u.if_go.op2);
            printf(" GOTO label%d\n", code->u.if_go.label_index);
            break;
        case RETURN:
            printf("RETURN ");
            print_var_or_constant(code->u.ret);
            printf("\n");
            break;
        case DEC:
            printf("DEC v%d %d\n", code->u.dec.x->u.var_no, code->u.dec.size * 4);
            break;
        case ARG:
            print_arg(code->u.args);
            break;
        case PARAM:
            print_param(code->u.param);
            break;
        case CALL:
            printf("v%d := CALL %s\n", code->u.call.ret->u.var_no, code->u.call.function_name);
            break;
        case READ:
            printf("READ v%d\n", code->u.rw->u.var_no);
            break;
        case WRITE:
            printf("WRITE v%d\n", code->u.rw->u.var_no);
            break;
        default:
            break;
        }
        head = head->next;
    }
}
//need to reconsider;
Operand *insert_variable(char *symbol_name, OperandKind kind)
{
    head->operand = malloc(sizeof(Operand));
    head->operand->kind = kind;
    head->name = symbol_name;
    head->operand->u.var_no = v_index++;
    Operand *o = head->operand;
    head->next = malloc(sizeof(Variable));
    head = head->next;
    return o;
}

void insert_function(char *function_name)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = IR_FUNCTION;
    code->u.function_name = function_name;
    insert_code(code);
    free(code);
}
//need to reconsider;
void insert_param(char *function_name)
{
    //TODO ADD PARAM -> VAR
    Symbol *s = SymbolGet(function_name, FUNCTION);
    if (IR_DEBUG)
    {
        if (s != NULL)
            printf("Get From Table Success!\n");
    }
    Field *param = s->type->field;
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = PARAM;
    // code->u.param = param;
    param = param->next;
    Variable *param_ = malloc(sizeof(Variable));
    code->u.param = param_;
    param_->operand = malloc(sizeof(Operand));
    while (param != NULL)
    {
        if (IR_DEBUG)
        {
            printf("%s\n", param->name);
        }
        Operand *o = insert_variable(param->name, VARIABLE);
        param_->operand = o;
        param_->next = malloc(sizeof(Variable));
        param_ = param_->next;
        param = param->next;
    }
    insert_code(code);
    free(code);
}

void gen_ir(int last_node)
{
    if (IR_DEBUG)
    {
        printf("Enter IR Gen Function!\n");
    }

    // translate_Program(last_node);

    // codes = codes->next;
    // if (IR_DEBUG)
    // {
    //     printf("Test IR\n");
    // }
    // while (codes != NULL && codes->code != NULL)
    // {
    //     printf("%d\n", codes->code->kind);
    //     codes = codes->next;
    // }

    print_ir();
}

void translate_Program(int index)
{
    ASTNode *program = nodes[index];
    DebugPrintNameType(program);
    translate_ExtDefList(program->child);
}

void translate_ExtDefList(int index)
{
    ASTNode *ext_def_list = nodes[index];
    DebugPrintNameType(ext_def_list);
    if (ext_def_list->type == ExtDefList_ExtDefExtDefList)
    {
        int ext_def = ext_def_list->child;
        translate_ExtDef(ext_def);
        int ext_def_list = nodes[ext_def]->brother;
        translate_ExtDefList(ext_def_list);
    }
}
void translate_ExtDef(int index)
{
    ASTNode *ext_def = nodes[index];
    DebugPrintNameType(ext_def);
    int *sons = GetSon(ext_def);
    switch (ext_def->type)
    {
    case ExtDef_SpecifierFunDecCompSt:
        translate_FunDec(sons[1]);
        translate_CompSt(sons[2]);
        break;
    default:
        break;
    }
}
//TODO
void translate_FunDec(int index)
{
    // ASTNode *fun_dec = nodes[index];
    // DebugPrintNameType(fun_dec);
    // int *sons = GetSon(fun_dec);
    PROCESS(fun_dec);
    char *name = nodes[sons[0]]->value;
    insert_function(name);
    switch (fun_dec->type)
    {
    case FunDec_IDLPVarListRP:
        // translate_VarList(sons[2]);
        insert_param(name);
        break;

    default:
        break;
    }
}

void translate_CompSt(int index)
{
    PROCESS(comt_st);
    translate_DefList(sons[1]);
    translate_StmtList(sons[2]);
}

void translate_StmtList(int index)
{
    PROCESS(stmt_list);
    if (stmt_list->type == StmtList_StmtStmtList)
    {
        translate_Stmt(sons[0]);
        translate_StmtList(sons[1]);
    }
}
//TODO
void translate_Stmt(int index)
{
    PROCESS(stmt);
    switch (stmt->type)
    {
    case Stmt_ExpSEMI:
        /* code */
        break;
    case Stmt_CompSt:
        translate_CompSt(sons[0]);
        break;
    case Stmt_ReturnExpSEMI:
        translate_Exp(sons[1]);
        break;
    case Stmt_IfLpExpRpStmt:
        translate_Exp(sons[2]);
        translate_Stmt(sons[4]);
        break;
    case Stmt_IfLpExpRpStmtElseStmt:
        translate_Exp(sons[2]);
        translate_Stmt(sons[4]);
        translate_Stmt(sons[6]);
        break;
    case Stmt_WhileLpExpRpStmt:
        translate_Exp(sons[2]);
        translate_Stmt(sons[4]);
        break;
    default:
        break;
    }
}

void translate_DefList(int index)
{
    PROCESS(def_list);
    if (def_list->type == DefList_DefDefList)
    {
        translate_Def(sons[0]);
        translate_DefList(sons[1]);
    }
}

void translate_Def(int index)
{
    PROCESS(def);
    translate_DecList(sons[1]);
}

void translate_DecList(int index)
{
    PROCESS(dec_list);
    switch (dec_list->type)
    {
    case DecList_Dec:
        translate_Dec(sons[0]);
        break;
    case DecList_DecCOMMADecList:
        translate_Dec(sons[0]);
        translate_DecList(sons[2]);
        break;
    default:
        break;
    }
}

void translate_Dec(int index)
{
    PROCESS(dec);
    switch (dec->type)
    {
    case Dec_VarDec:
        translate_VarDec(sons[0]);
        break;
    case Dec_VarDecASSIGNOPExp:
        translate_VarDec(sons[0]);
        translate_Exp(sons[2]);
    default:
        break;
    }
}
//TODO
Operand *translate_VarDec(int index)
{
    PROCESS(var_dec);
    switch (var_dec->type)
    {
    case VarDec_ID:
    {
        char *name = nodes[sons[0]]->value;
        Symbol *s = SymbolGet(name, VAR);

        break;
    }
    case VarDec_VarDecLbIntRb:
    default:
        break;
    }
}
//TODO
Operand *translate_Exp(int index)
{
    PROCESS(exp);
}
void translate_Cond(int, int, int);
void translate_Args(int);
