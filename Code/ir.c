#include "ir.h"

int v_index = 1;
int label_index = 1;
Symbol *now_stack;
int now_depth = 0;
int now_size = 0;
int whether_optimize = 0;
void insert_code(InterCode *code)
{
    // now->code = malloc(sizeof(InterCode));
    now->code = code;
    now->next = malloc(sizeof(InterCodes));
    now->next->prev = now;
    now = now->next;
    now->code = NULL;
    now->next = NULL;
    // printf("%d\n", code->kind);
    // printf("Insert Code Success\n");
}

char *find_v(int v)
{
    Variable *temp = symbol_to_operand->next;
    while (temp != NULL && temp->operand != NULL)
    {
        if (temp->operand->kind != CONSTANT && v == temp->operand->u.var_no)
            return temp->name;
        else
            temp = temp->next;
    }
    return NULL;
}

void print_var_or_constant_or_addr(Operand *o, FILE *f, int flag)
{
    if (o->kind == CONSTANT)
        fprintf(f, "#%d", o->u.value);
    else if (o->kind == ADDRESS)
    {
        fprintf(f, "v%d", o->u.var_no);
    }
    else
        fprintf(f, "v%d", o->u.var_no);
}

void print_binop_(Operand *o, FILE *f, int flag)
{
    if (o->kind == CONSTANT)
        fprintf(f, "#%d", o->u.value);
    else if (o->kind == ADDRESS)
    {
        fprintf(f, "*v%d", o->u.var_no);
    }
    else
        fprintf(f, "v%d", o->u.var_no);
}

void if_else_print(Operand *o, FILE *f)
{
    if (o->kind == CONSTANT)
        fprintf(f, "#%d", o->u.value);
    else if (o->kind == ADDRESS)
    {
        fprintf(f, "*v%d", o->u.var_no);
    }
    else
        fprintf(f, "v%d", o->u.var_no);
}

void print_binop(InterCode *code, char binop, FILE *f)
{
    Operand *left = code->u.binop.op1;
    Operand *right = code->u.binop.op2;
    Operand *result = code->u.binop.result;
    if (result->kind == left->kind && left->kind != CONSTANT)
    {
        fprintf(f, "v%d := v%d %c ", result->u.var_no, left->u.var_no, binop);
        print_binop_(right, f, 1);
        fprintf(f, "\n");
    }
    else
    {
        print_binop_(result, f, 0);
        fprintf(f, " := ");
        print_binop_(left, f, 1);
        fprintf(f, " %c ", binop);
        print_binop_(right, f, 1);
        fprintf(f, "\n");
    }
    // fprintf(f, "v%d := ", result->u.var_no);
}

void print_arg(Operand *arg, TypeKind kind, FILE *f)
{
    if (kind == STRUCTURE)
    {
        if (arg->kind == VARIABLE)
            fprintf(f, "&v%d", arg->u.var_no);
        if (arg->kind == ADDRESS)
            fprintf(f, "v%d", arg->u.var_no);
    }
    else
    {
        if (arg->kind == CONSTANT)
            fprintf(f, "#%d", arg->u.value);
        else
            fprintf(f, "v%d", arg->u.var_no);
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

void print_code(InterCode *code, FILE *f)
{
    switch (code->kind)
    {
    case LABEL:
    {
        fprintf(f, "LABEL label%d :\n", code->u.label_index);
        break;
    }
    case IR_FUNCTION:
        fprintf(f, "FUNCTION %s :\n", code->u.function_name);
        break;
    case ASSIGN:
    {
        Operand *left = code->u.assign.left;
        Operand *right = code->u.assign.right;
        // print_var_or_constant_or_addr(left, f, -1);
        fprintf(f, "v%d", left->u.var_no);
        fprintf(f, " := ");
        print_var_or_constant_or_addr(right, f, 1);
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
    case ADDR_ASSIGN:
    {
        Operand *left = code->u.binop.result;
        Operand *right = code->u.binop.op1;
        Operand *constant = code->u.binop.op2;
        fprintf(f, "v%d := ", left->u.var_no);
        if (right->kind == VARIABLE)
        {
            fprintf(f, "&v%d", right->u.var_no);
        }
        else if (right->kind == ADDRESS)
        {
            fprintf(f, "v%d", right->u.var_no);
        }
        if (constant != NULL)
        {
            if (constant->kind == CONSTANT)
            {
                fprintf(f, " + #%d", constant->u.value);
            }
            if (constant->kind == VARIABLE)
            {
                fprintf(f, " + v%d", constant->u.var_no);
            }
        }
        fprintf(f, "\n");
        break;
    }
    case ASSIGN_DEREFERENCE:
    {
        Operand *left = code->u.assign.left;
        Operand *right = code->u.assign.right;
        fprintf(f, "v%d := *v%d\n", left->u.var_no, right->u.var_no);
        break;
    }
    case DEREFERENCE_ASSIGN:
    {
        Operand *left = code->u.assign.left;
        Operand *right = code->u.assign.right;
        fprintf(f, "*v%d := ", left->u.var_no);
        if (right->kind == VARIABLE)
            fprintf(f, "v%d", right->u.var_no);
        else if (right->kind == CONSTANT)
            fprintf(f, "#%d", right->u.value);
        else
            fprintf(f, "*v%d", right->u.var_no);
        fprintf(f, "\n");
        break;
    }
    case GOTO:
        fprintf(f, "GOTO label%d\n", code->u.label_index);
        break;
    case IF_GOTO:
        fprintf(f, "IF ");
        if_else_print(code->u.if_go.op1, f);
        fprintf(f, " %s ", code->u.if_go.relop);
        if_else_print(code->u.if_go.op2, f);
        fprintf(f, " GOTO label%d\n", code->u.if_go.label_index);
        break;
    case RETURN:
        fprintf(f, "RETURN ");
        print_binop_(code->u.ret, f, 1);
        fprintf(f, "\n");
        break;
    case DEC:
        fprintf(f, "DEC v%d %d\n", code->u.dec.x->u.var_no, code->u.dec.size * 4);
        break;
    case ARG:
        // print_arg(code->u.args);
        fprintf(f, "ARG ");
        //TODO
        print_arg(code->u.arg.arg, code->u.arg.kind, f);
        // print_var_or_constant_or_addr(code->u.arg, f, 1);
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
        fprintf(f, "WRITE ");
        if (code->u.rw->kind == ADDRESS)
            fprintf(f, "*");
        print_var_or_constant_or_addr(code->u.rw, f, 0);
        fprintf(f, "\n");
        break;
    default:
        break;
    }
}

void print_ir(FILE *f)
{
    InterCodes *head = codes->next;
    while (head != NULL && head->code != NULL)
    {
        InterCode *code = head->code;
        print_code(code, f);
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
        if (param->type->kind == STRUCTURE)
            o->kind = ADDRESS;
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
    if (arg->kind == CONSTANT)
    {
        code->u.arg.arg = arg;
        insert_code(code);
    }
    else
    {
        char *name = find_v(arg->u.var_no);
        code->u.arg.arg = arg;
        Symbol *s = SymbolGet(name, VAR);
        if (s == NULL)
            code->u.arg.kind = 0;
        else
            code->u.arg.kind = SymbolGet(name, VAR)->type->kind;
        insert_code(code);
    }
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
    // if (left->kind != CONSTANT && right->kind != CONSTANT)
    // {
    //     if (left->u.var_no == right->u.var_no)
    //     {
    //         free(code);
    //         return;
    //     }
    // }
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
    if (result->kind != ADDRESS && left->kind != ADDRESS && right->kind != ADDRESS)
    {
        if (right->kind == CONSTANT)
        {
            if ((right->u.value == 0) && (code->kind == ADD || code->kind == SUB))
            {
                free(code);
                insert_assign(result, left);
                return;
            }
            if ((right->u.value == 1) && (code->kind == MUL || code->kind == DIV))
            {
                free(code);
                insert_assign(result, left);
                return;
            }
        }
        if (code->kind == DIV && left->u.var_no == right->u.var_no)
        {
            code->u.binop.op1 = get_constant(1);
            code->u.binop.op2 = get_constant(1);
            insert_code(code);
            return;
        }
    }
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

void insert_dec(Operand *o, int size)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = DEC;
    code->u.dec.size = size;
    code->u.dec.x = o;
    insert_code(code);
}

void insert_address_assign(Operand *left, Operand *right, int index)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = ADDR_ASSIGN;
    if (index == 0)
    {
        code->u.binop.op1 = right;
        code->u.binop.op2 = NULL;
        code->u.binop.result = left;
    }
    else
    {
        code->u.binop.op1 = right;
        code->u.binop.op2 = get_constant(index);
        code->u.binop.result = left;
    }
    insert_code(code);
}

void insert_array(Operand *left, Operand *right, Operand *index)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = ADDR_ASSIGN;
    if (index == NULL)
    {
        code->u.binop.op1 = right;
        code->u.binop.op2 = NULL;
        code->u.binop.result = left;
    }
    else if (index->kind == CONSTANT)
    {
        code->u.binop.op1 = right;
        code->u.binop.op2 = index;
        code->u.binop.result = left;
    }
    else if (index->kind == VARIABLE)
    {
        code->u.binop.op1 = right;
        code->u.binop.op2 = index;
        code->u.binop.result = left;
    }
    insert_code(code);
}
//v = *v;
void insert_assign_dereference(Operand *left, Operand *right)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->u.assign.left = left;
    code->u.assign.right = right;
    code->kind = ASSIGN_DEREFERENCE;
    insert_code(code);
}
//*v = v;
void insert_dereference_assign(Operand *left, Operand *right)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->u.assign.left = left;
    code->u.assign.right = right;
    code->kind = DEREFERENCE_ASSIGN;
    insert_code(code);
}

// without * 4;
int sizeofStruct(Field *field)
{
    int size = 0;
    while (field != NULL)
    {
        if (field->type->kind == BASIC)
        {
            size++;
        }
        else if (field->type->kind == ARRAY)
        {
            if (field->type->array->type->kind == BASIC)
            {
                size += field->type->array->size;
            }
            else
            {
                size += (field->type->array->size) * sizeofStruct(field->type->array->type->field);
            }
        }
        else
        {
            size += sizeofStruct(field->type->field);
        }
        field = field->next;
    }
    return size;
}

int sizeofArrayItem(char *name)
{
    Symbol *s = SymbolGet(name, VAR);
    if (s == NULL)
        s = SymbolGet(name, FIELD);
    if (s->type->array->type->kind == BASIC)
    {
        return 4;
    }
    else
    {
        return sizeofStruct(s->type->array->type->field) * 4;
    }
}

Symbol *find_struct(char *name)
{
    for (int i = 0; i < nodes_point; i++)
    {
        if (symbol_table[i]->lineno == -1)
            continue;
        else
        {
            Symbol *symbol = symbol_table[i];
            while (symbol != NULL)
            {
                if (symbol->kind == STRUCT)
                {
                    Field *field = symbol->type->field;
                    while (field != NULL)
                    {
                        if (strcmp(field->name, name) == 0)
                            return symbol;
                        else
                            field = field->next;
                    }
                }
                symbol = symbol->next;
            }
        }
    }
    return NULL;
}

int cal_index(char *field)
{
    Field *struct_field = now_stack->type->field;
    int size = 0;
    while (struct_field != NULL)
    {
        if (strcmp(field, struct_field->name) == 0)
            return size;
        else
        {
            if (struct_field->type->kind == BASIC)
                size += 4;
            else if (struct_field->type->kind == ARRAY)
            {
                size += struct_field->type->array->size * sizeofArrayItem(struct_field->name);
            }
            else
            {
                size += sizeofStruct(struct_field) * 4;
            }
        }
        struct_field = struct_field->next;
    }
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
    FILE *f = fopen("/dev/null", "w+");
    print_ir(f);
    fclose(f);
    free(nodes);
    // free(symbol_table);
    if (whether_optimize == 0)
    {
        optimize(v_index, "out.ir");
    }
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

char *get_exp_name(int index)
{
    PROCESS(exp);
    switch (exp->type)
    {
    case Exp_Id:
        return nodes[sons[0]]->value;
    case Exp_ExpLbExpRb:
    {
        return get_exp_name(sons[0]);
    }
    case Exp_ExpDotId:
        return get_exp_name(sons[0]);
    default:
        break;
    }
}

char *get_array_name(int index)
{
    PROCESS(exp);
    switch (exp->type)
    {
    case Exp_Id:
        return nodes[sons[0]]->value;
        break;
    case Exp_ExpDotId:
        return nodes[sons[2]]->value;
    default:
        break;
    }
    return NULL;
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
        if (l->kind == ADDRESS)
            insert_dereference_assign(l, r);
        else if (r->kind == ADDRESS)
            insert_assign_dereference(l, r);
        else
            insert_assign(l, r);
        // insert_assign(l, r);
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
        Symbol *s = SymbolGet(name, VAR);
        if (s->type->kind == STRUCTURE)
        {
            whether_optimize = 1;
            Operand *o = insert_variable_by_name(name, VARIABLE);
            insert_dec(o, sizeofStruct(s->type->field));
        }
        // if (s->type->kind == ARRAY)
        // {
        //     Operand* o = insert_variable_by_name(name, VARIABLE);
        // }
        return insert_variable_by_name(name, VARIABLE);
    }
    case VarDec_VarDecLbIntRb:
    {
        if (nodes[sons[0]]->type == VarDec_VarDecLbIntRb)
        {
            printf("error! meet High Dimensional Array\n");
            exit(0);
        }
        Operand *o = translate_VarDec(sons[0]);
        int size = IntAnalyze(sons[2]);
        char *array_name = nodes[nodes[sons[0]]->child]->value;
        Symbol *s = SymbolGet(array_name, VAR);
        if (s->type->array->type->kind == STRUCTURE)
        {
            size = size * sizeofStruct(s->type->array->type->field);
        }
        insert_dec(o, size);
        return o;
    }
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
        if (l->kind == ADDRESS)
            insert_dereference_assign(l, r);
        else if (r->kind == ADDRESS)
            insert_assign_dereference(l, r);
        else
        {
            if (l->kind == ADDRESS && r->kind == ADDRESS)
            {
                insert_assign(l, r);
            }
            else
            {
                if (get_array_name(sons[0]) != NULL && get_array_name(sons[2]) != NULL)
                {
                    Symbol *a1 = SymbolGet(get_array_name(sons[0]), VARIABLE);
                    Symbol *a2 = SymbolGet(get_array_name(sons[2]), VARIABLE);
                    if (a1->type->kind == ARRAY && a2->type->kind == ARRAY)
                    {
                        int asize = a1->type->array->size;
                        int bsize = a2->type->array->size;
                        Operand *a1o = malloc(sizeof(Operand));
                        Operand *a2o = malloc(sizeof(Operand));
                        insert_variable_by_ope(a1o, ADDRESS);
                        insert_variable_by_ope(a2o, ADDRESS);
                        for (int i = 0; i < asize; i++)
                        {
                            insert_array(a1o, l, get_constant(i * sizeofArrayItem(a1->name)));
                            if (i < bsize)
                            {
                                insert_array(a2o, r, get_constant(i * sizeofArrayItem(a2->name)));
                                insert_dereference_assign(a1o, a2o);
                            }
                            else
                                insert_dereference_assign(a1o, get_constant(0));
                        }
                    }
                    else insert_assign(l, r);
                }
                else
                    insert_assign(l, r);
            }
        }
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
    {
        return translate_Exp(sons[1]);
    }
    case Exp_MinusExp:
    {
        Operand *o = translate_Exp(sons[1]);
        Operand *c = get_constant(0);
        Operand *result = malloc(sizeof(Operand));
        insert_variable_by_ope(result, VARIABLE);
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
        if (IR_DEBUG)
            printf("%s\n", nodes[sons[0]]->value);
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
    case Exp_ExpLbExpRb:
    {
        Operand *o1 = translate_Exp(sons[0]);
        Operand *o2 = translate_Exp(sons[2]);
        Operand *result = malloc(sizeof(Operand));
        insert_variable_by_ope(result, ADDRESS);
        // printf("%s\n", now_stack->name);
        int size = 0;
        if (o1->kind == VARIABLE)
        {
            size = sizeofArrayItem(get_exp_name(sons[0]));
        }
        if (o1->kind == ADDRESS)
        {
            char *array_name = get_array_name(sons[0]);
            size = sizeofArrayItem(array_name);
        }
        if (IR_DEBUG)
            printf("%d\n", size);
        Operand *o3 = malloc(sizeof(Operand));
        insert_variable_by_ope(o3, VARIABLE);
        insert_binop(o3, o2, get_constant(size), "STAR");
        insert_array(result, o1, o3);
        return result;
    }
    case Exp_ExpDotId:
    {
        whether_optimize = 1;
        now_depth++;
        Operand *o = translate_Exp(sons[0]);
        char *field = nodes[sons[2]]->value;
        now_stack = find_struct(field);
        int index = cal_index(field);
        Operand *addr = malloc(sizeof(Operand));
        insert_variable_by_ope(addr, ADDRESS);
        if (o->kind == VARIABLE)
        {
            insert_address_assign(addr, o, index);
        }
        if (o->kind == ADDRESS)
        {
            insert_binop(addr, o, get_constant(index), "PLUS");
        }
        return addr;
        // if (nodes[sons[0]]->type != Exp_ExpDotId) // 递归到最里面
        // {
        //     // char *name = get_exp_name(sons[0]);
        //     printf("field %s\n", field);
        //     now_stack = find_struct(field);
        //     printf("struct %s\n", now_stack->name);
        //     now_size = 0;
        //     // 不需要递归
        //     if (now_depth == 1)
        //     {
        //         int index = cal_index(field);
        //         printf("index %d\n", index);
        //         now_depth = 0;
        //         Operand *addr = malloc(sizeof(Operand));
        //         insert_variable_by_ope(addr, ADDRESS);
        //         if (o->kind == VARIABLE)
        //         {
        //             insert_address_assign(addr, o, index);
        //         }
        //         if (o->kind == ADDRESS)
        //         {
        //             insert_binop(addr, o, get_constant(index), "PLUS");
        //         }
        //         return addr;
        //     }
        //     else
        //     {
        //         now_size += cal_index(field);
        //         printf("index %d\n", now_size);
        //         Symbol *field_symbol = SymbolGet(field, FIELD);
        //         if (field_symbol->type->kind == STRUCTURE)
        //             now_stack = field_symbol;
        //     }
        //     return o;
        // }
        // else
        // {
        //     //这里需要能判断到了结构体的最外层
        //     now_depth--;
        //     Symbol *field_symbol = SymbolGet(field, FIELD);
        //     now_size += cal_index(field);
        //     if (now_depth == 1)
        //     {
        //         now_depth = 0;
        //         Operand *addr = malloc(sizeof(Operand));
        //         insert_variable_by_ope(addr, ADDRESS);
        //         if (o->kind == VARIABLE)
        //         {
        //             insert_address_assign(addr, o, now_size);
        //         }
        //         if (o->kind == ADDRESS)
        //         {
        //             insert_binop(addr, o, get_constant(now_size), "PLUS");
        //         }
        //         return addr;
        //     }
        //     if (field_symbol->type->kind == STRUCTURE)
        //     {
        //         now_stack = field_symbol;
        //     }
        //     return o;
        // }
    }
    case Exp_Id:
    {
        char *name = nodes[sons[0]]->value;
        if (IR_DEBUG)
        {
            printf("%s\n", name);
        }
        Operand *o = get_variable(name);
        return o;
    }
    case Exp_Int:
    {
        Operand *o = malloc(sizeof(Operand));
        o->kind = CONSTANT;
        o->u.value = IntAnalyze(sons[0]);
        if (IR_DEBUG)
        {
            printf("%d\n", o->u.value);
        }
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
    if (IR_DEBUG)
        printf("Enter Cond\n");
    switch (exp->type)
    {
    case Exp_ExpAndExp:
    {
        int label = get_label();
        translate_Cond(sons[0], label, false_label);
        insert_label(label);
        translate_Cond(sons[2], true_label, false_label);
        break;
    }
    case Exp_ExpOrExp:
    {
        int label = get_label();
        translate_Cond(sons[0], true_label, label);
        insert_label(label);
        translate_Cond(sons[2], true_label, false_label);
        break;
    }
    case Exp_ExpRelopExp:
    {
        Operand *o1 = translate_Exp(sons[0]);
        Operand *o2 = translate_Exp(sons[2]);
        char *relop = nodes[sons[1]]->value;
        if (IR_DEBUG)
            printf("relop: %s\n", relop);
        insert_ifgoto(o1, o2, relop, true_label);
        insert_goto(false_label);
        break;
    }
    case Exp_NotExp:
    {
        translate_Cond(sons[1], false_label, true_label);
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
