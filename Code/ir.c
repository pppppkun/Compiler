#include "ir.h"

int v_index = 1;
int label_index = 1;

void insert_code(InterCode *code)
{
    now->code = malloc(sizeof(InterCode));
    now->code = code;
    now->next = malloc(sizeof(InterCodes));
    now->next->prev = now;
    now = now->next;
    now->next = NULL;
    // printf("%d\n", code->kind);
    // printf("Insert Code Success\n");
}

void print_var_or_constant(Operand *o, FILE *f)
{
    if (o->kind == CONSTANT)
        fprintf(f, "#%d", o->u.value);
    else
        fprintf(f, "v%d", o->u.var_no);
}

void print_binop(InterCode *code, char binop, FILE *f)
{
    Operand *left = code->u.binop.op1;
    Operand *right = code->u.binop.op2;
    Operand *result = code->u.binop.result;
    fprintf(f, "v%d := ", result->u.var_no);
    print_var_or_constant(left, f);
    fprintf(f, " %c ", binop);
    print_var_or_constant(right, f);
    fprintf(f, "\n");
}

void print_arg(Variable *args)
{
    while (args != NULL && args->operand != NULL)
    {
        printf("ARG v%d\n", args->operand->u.var_no);
        args = args->prev;
    }
}

void print_param(Variable *param, FILE *f)
{
    while (param != NULL && param->operand != NULL)
    {
        fprintf(f, "PARAM v%d\n", param->operand->u.var_no);
        param = param->next;
    }
}

void print_ir(FILE *f)
{
    InterCodes *head = codes->next;
    while (head != NULL && head->code != NULL)
    {
        InterCode *code = head->code;
        switch (code->kind)
        {
        case LABEL:
        {
            fprintf(f, "LABEL label%d :\n", code->u.label_index);
            break;
        }
        case IR_FUNCTION:
            fprintf(f, "FUNCTION %s :\n", head->code->u.function_name);
            break;
        case ASSIGN:
        {
            Operand *left = code->u.assign.left;
            Operand *right = code->u.assign.right;
            fprintf(f, "v%d := ", left->u.var_no);
            print_var_or_constant(right, f);
            fprintf(f, "\n");
            break;
        }
        case ADD:
            print_binop(code, '+', f);
            break;
        case SUB:
            print_binop(code, '-', f);
            break;
        case MUL:
            print_binop(code, '*', f);
            break;
        case DIV:
            print_binop(code, '/', f);
            break;
        case GOTO:
            fprintf(f, "GOTO label%d\n", code->u.label_index);
            break;
        case IF_GOTO:
            fprintf(f, "IF ");
            print_var_or_constant(code->u.if_go.op1, f);
            fprintf(f, " %s ", code->u.if_go.relop);
            print_var_or_constant(code->u.if_go.op2, f);
            fprintf(f, " GOTO label%d\n", code->u.if_go.label_index);
            break;
        case RETURN:
            fprintf(f, "RETURN ");
            print_var_or_constant(code->u.ret, f);
            fprintf(f, "\n");
            break;
        case DEC:
            fprintf(f, "DEC v%d %d\n", code->u.dec.x->u.var_no, code->u.dec.size * 4);
            break;
        case ARG:
            // print_arg(code->u.args);
            fprintf(f, "ARG ");
            print_var_or_constant(code->u.arg, f);
            fprintf(f, "\n");
            break;
        case PARAM:
            print_param(code->u.param, f);
            break;
        case CALL:
            fprintf(f, "v%d := CALL %s\n", code->u.call.ret->u.var_no, code->u.call.function_name);
            break;
        case READ:
            fprintf(f, "READ v%d\n", code->u.rw->u.var_no);
            break;
        case WRITE:
            fprintf(f, "WRITE v%d\n", code->u.rw->u.var_no);
            break;
        default:
            break;
        }
        head = head->next;
    }
}
//need to reconsider
Operand *insert_variable_by_name(char *symbol_name, OperandKind kind)
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

Operand *insert_variable_by_ope(Operand *operand, OperandKind kind)
{
    head->operand = operand;
    head->operand->kind = kind;
    head->name = random();
    head->operand->u.var_no = v_index++;
    head->next = malloc(sizeof(Variable));
    head = head->next;
    return operand;
}

Operand *get_variable(char *symbol_name)
{
    Variable *temp = symbol_to_operand->next;
    while (temp != NULL && temp->operand != NULL)
    {
        if (strcmp(temp->name, symbol_name) == 0)
            return temp->operand;
        else
            temp = temp->next;
    }
    return NULL;
}

Operand *get_constant(int value)
{
    Operand *constant = malloc(sizeof(Operand));
    constant->kind = CONSTANT;
    constant->u.value = value;
    return constant;
}

int get_label()
{
    return label_index++;
}

void insert_function(char *function_name)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = IR_FUNCTION;
    code->u.function_name = function_name;
    insert_code(code);
    // free(code);
}

void insert_param(char *function_name)
{

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
        Operand *o = insert_variable_by_name(param->name, VARIABLE);

        param_->operand = o;
        param_->next = malloc(sizeof(Variable));
        param_->next->operand = NULL;

        param_ = param_->next;
        param = param->next;
    }
    insert_code(code);
    // free(code);
}

void insert_arg(Operand *arg)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = ARG;
    code->u.arg = arg;
    insert_code(code);
}

void insert_call(Operand *ret, char *name)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = CALL;
    code->u.call.function_name = name;
    code->u.call.ret = ret;
    insert_code(code);
}

void insert_assign(Operand *left, Operand *right)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = ASSIGN;
    code->u.assign.left = left;
    code->u.assign.right = right;
    insert_code(code);
    // free(code);
}

void insert_label(int index)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = LABEL;
    code->u.label_index = index;
    insert_code(code);
}

void insert_binop(Operand *result, Operand *left, Operand *right, char *binop)
{
    InterCode *code = malloc(sizeof(InterCode));
    if (strcmp(binop, "PLUS") == 0)
        code->kind = ADD;
    if (strcmp(binop, "MINUS") == 0)
        code->kind = SUB;
    if (strcmp(binop, "STAR") == 0)
        code->kind = MUL;
    if (strcmp(binop, "DIV") == 0)
        code->kind = DIV;
    code->u.binop.op1 = left;
    code->u.binop.op2 = right;
    code->u.binop.result = result;
    insert_code(code);
}

void insert_write(Operand *arg)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = WRITE;
    code->u.rw = arg;
    insert_code(code);
}

void insert_read(Operand *arg)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = READ;
    code->u.rw = arg;
    insert_code(code);
}

void insert_return(Operand *ret)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = RETURN;
    code->u.ret = ret;
    insert_code(code);
}

void insert_goto(int label_index)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = GOTO;
    code->u.label_index = label_index;
    insert_code(code);
}

void insert_ifgoto(Operand *o1, Operand *o2, char *relop, int label_index)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = IF_GOTO;
    code->u.if_go.label_index = label_index;
    code->u.if_go.op1 = o1;
    code->u.if_go.op2 = o2;
    code->u.if_go.relop = relop;
    insert_code(code);
}

// void insert_AndOr(Operand *left, Operand *right, int flag)
// {
//     InterCode *code = malloc(sizeof(InterCode));
//     code->kind =
// }

void gen_ir(int last_node, char *file_name)
{
    if (IR_DEBUG)
    {
        printf("Enter IR Gen Function!\n");
    }
    codes = malloc(sizeof(InterCodes));
    now = malloc(sizeof(InterCodes));
    codes->next = now;
    now->prev = codes;
    now->next = NULL;
    symbol_to_operand = malloc(sizeof(Variable));
    head = malloc(sizeof(Variable));
    symbol_to_operand->next = head;

    translate_Program(last_node);
    FILE *f = fopen(file_name, "w+");
    print_ir(f);
    // codes = codes->next;
    if (IR_DEBUG)
    {
        printf("Test IR\n");
        symbol_to_operand = symbol_to_operand->next;
        while (symbol_to_operand != NULL && symbol_to_operand->operand != NULL)
        {
            printf("name: %s variable: v%d\n", symbol_to_operand->name, symbol_to_operand->operand->u.var_no);
            symbol_to_operand = symbol_to_operand->next;
        }
    }
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

void translate_FunDec(int index)
{
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
        translate_Exp(sons[0]);
        break;
    case Stmt_CompSt:
        translate_CompSt(sons[0]);
        break;
    case Stmt_ReturnExpSEMI:
        insert_return(translate_Exp(sons[1]));
        break;
    case Stmt_IfLpExpRpStmt:
    {
        int true_label = get_label();
        int false_label = get_label();
        translate_Cond(sons[2], true_label, false_label);
        insert_label(true_label);
        translate_Stmt(sons[4]);
        insert_label(false_label);
        break;
    }
    case Stmt_IfLpExpRpStmtElseStmt:
    {
        int true_label = get_label();
        int false_label = get_label();
        int out_label = get_label();
        translate_Cond(sons[2], true_label, false_label);
        insert_label(true_label);
        translate_Stmt(sons[4]);
        insert_goto(out_label);
        insert_label(false_label);
        translate_Stmt(sons[6]);
        insert_label(out_label);
        break;
    }
    case Stmt_WhileLpExpRpStmt:
    {
        int boolean_label = get_label();
        int stmt_label = get_label();
        int out_label = get_label();
        insert_label(boolean_label);
        translate_Cond(sons[2], stmt_label, out_label);
        insert_label(stmt_label);
        translate_Stmt(sons[4]);
        insert_goto(boolean_label);
        insert_label(out_label);
        break;
    }
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
    {
        Operand *l = translate_VarDec(sons[0]);
        Operand *r = translate_Exp(sons[2]);
        insert_assign(l, r);
    }
    default:
        break;
    }
}

Operand *translate_VarDec(int index)
{
    PROCESS(var_dec);
    switch (var_dec->type)
    {
    case VarDec_ID:
    {
        char *name = nodes[sons[0]]->value;
        return insert_variable_by_name(name, VARIABLE);
    }
    case VarDec_VarDecLbIntRb: //don't consider.
    default:
        break;
    }
}
//TODO
Operand *translate_Exp(int index)
{
    PROCESS(exp);
    switch (exp->type)
    {
    case Exp_ExpAssignopExp:
    {
        Operand *l = translate_Exp(sons[0]);
        Operand *r = translate_Exp(sons[2]);
        insert_assign(l, r);
        return l;
    }
    case Exp_ExpAndExp:
    case Exp_ExpOrExp:
    case Exp_ExpRelopExp:
    case Exp_NotExp:
    {
        int true_label = get_label();
        int false_label = get_label();
        Operand *place = malloc(sizeof(Operand));
        insert_variable_by_ope(place, VARIABLE);
        insert_assign(place, get_constant(0));
        translate_Cond(index, true_label, false_label);
        insert_label(true_label);
        insert_assign(place, get_constant(1));
        insert_label(false_label);
        return place;
    }
    case Exp_ExpPlusExp:
    case Exp_ExpMinusExp:
    case Exp_ExpStarExp:
    case Exp_ExpDivExp:
    {
        Operand *left = translate_Exp(sons[0]);
        Operand *right = translate_Exp(sons[2]);
        Operand *result = malloc(sizeof(Operand));
        insert_variable_by_ope(result, VARIABLE);
        insert_binop(result, left, right, nodes[sons[1]]->name);
        return result;
    }
    case Exp_LpExpRp:
        break;
    case Exp_MinusExp:
    {
        Operand *o = translate_Exp(sons[1]);
        Operand *c = get_constant(0);
        Operand *result = malloc(sizeof(Operand));
        insert_binop(result, c, o, "MINUS");
        return result;
    }
    case Exp_IdLpArgsRp:
    {
        if (strcmp(nodes[sons[0]]->value, "write") == 0)
        {
            Operand *arg = translate_Exp(nodes[sons[2]]->child);
            insert_write(arg);
            return get_constant(0);
        }
        translate_Args(sons[2]);
        Operand *ret = malloc(sizeof(Operand));
        ret = insert_variable_by_ope(ret, VARIABLE);
        insert_call(ret, nodes[sons[0]]->value);
        return ret;
    }
    case Exp_IdLpRp:
    {
        if (strcmp(nodes[sons[0]]->value, "read") == 0)
        {
            Operand *arg = malloc(sizeof(Operand));
            insert_variable_by_ope(arg, VARIABLE);
            insert_read(arg);
            return arg;
        }
        Operand *ret = malloc(sizeof(Operand));
        ret = insert_variable_by_ope(ret, VARIABLE);
        insert_call(ret, nodes[sons[0]]->value);
        return ret;
    }
    case Exp_ExpLbExpRb: //don't consider
        break;
    case Exp_ExpDotId: //wait.
    {
        break;
    }
    case Exp_Id:
    {
        char *name = nodes[sons[0]]->value;
        Operand *o = get_variable(name);
        return o;
    }
    case Exp_Int:
    {
        Operand *o = malloc(sizeof(Operand));
        o->kind = CONSTANT;
        o->u.value = IntAnalyze(sons[0]);
        return o;
    }
    default:
        return NULL;
    }
}
//TODO
void translate_Cond(int index, int true_label, int false_label)
{
    PROCESS(exp);
    switch (exp->type)
    {
    case Exp_ExpAndExp:
    {
        int label = get_label();
        translate_Cond(sons[0], label, false_label);
        insert_label(label);
        translate_Cond(sons[1], true_label, false_label);
        break;
    }
    case Exp_ExpOrExp:
    {
        int label = get_label();
        translate_Cond(sons[0], true_label, label);
        insert_label(label);
        translate_Cond(sons[1], true_label, false_label);
        break;
    }
    case Exp_ExpRelopExp:
    {
        Operand *o1 = translate_Exp(sons[0]);
        Operand *o2 = translate_Exp(sons[2]);
        char *relop = nodes[sons[1]]->value;
        insert_ifgoto(o1, o2, relop, true_label);
        insert_goto(false_label);
        break;
    }
    case Exp_NotExp:
    {
        translate_Cond(index, false_label, true_label);
        break;
    }
    default:
    {
        Operand *o = translate_Exp(index);
        insert_ifgoto(o, get_constant(0), "!=", true_label);
        insert_goto(false_label);
        break;
    }
    }
}
void translate_Args(int index)
{
    PROCESS(args);
    Variable *narg = malloc(sizeof(Variable));
    switch (args->type)
    {
    case Args_ExpCommaArgs:
    {
        translate_Args(sons[2]);
        Operand *e = translate_Exp(sons[0]);
        insert_arg(e);
        break;
    }
    case Args_Exp:
    {
        Operand *e = translate_Exp(sons[0]);
        insert_arg(e);
        break;
    }
    }
}
