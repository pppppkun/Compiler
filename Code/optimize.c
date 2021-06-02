#include "global.h"
#include "global_ir.h"

typedef struct BasicBlock BB;
typedef struct DAG DAG;
typedef struct DAGNode DAGNode;
typedef struct CFGNode CFGNode;
typedef struct AvailableExpression AE;
typedef enum DAGNodeKind DAG_KIND;

struct BasicBlock
{
    InterCodes *start;
    InterCodes *end;
    int size;
};

enum DAGNodeKind
{
    DAG_VAR,
    DAG_CONSTANT,
    DAG_OP_WITH_VAR
};

struct DAGNode
{
    // kind of node
    DAG_KIND kind;
    // unique identifier of var
    int v_index;
    // constants
    int v_value;
    // operator symbol
    int op_kind;
    // line
    int lineno;
    // reference
    int reference;
    //
    int last_modify_line;
    DAGNode *left;
    DAGNode *right;
};

struct AvailableExpression
{
    int lineno;
    Operand *op1;
    int op_kind;
    Operand *op2;
    Operand *res;
    AE *next;
    AE *prev;
};

struct CFGNode
{
    int size;
    BB **in;
};

DAGNode **var_dag;
DAGNode **constant_dag;
CFGNode *cfg;
InterCodes **leaders;
AE *aes;
AE *next;
BB **blocks;
int *if_go;

int var_size;
int constant_size;
int block_size;
int leader_size;
int aes_index;

int insert_leaders(InterCodes *c, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (leaders[i]->code->lineno == c->code->lineno)
        {
            return 1;
        }
    }
    leaders[size] = c;
    return 0;
}

int is_leaders(InterCodes *c, int size)
{
    int ret = 1;
    for (int i = 0; i < size; i++)
    {
        if (leaders[i]->code->lineno == c->code->lineno)
        {
            ret = 0;
            break;
        }
    }
    return ret;
}

InterCodes *find_label(int label_index)
{
    InterCodes *head = codes->next;
    while (head != NULL && head->code != NULL)
    {
        if (head->code->kind == LABEL)
        {
            if (head->code->u.label_index == label_index)
                return head;
        }
        head = head->next;
    }
    return NULL;
}

void print_block(BB *block, FILE *f)
{
    // printf("--block start--\n");
    InterCodes *s = block->start;
    print_code(s->code, f);
    while (s != block->end)
    {
        s = s->next;
        print_code(s->code, f);
    }
    // printf("--block end--\n");
}

void print_blocks(FILE *f)
{
    for (int i = 0; i < block_size; i++)
        print_block(blocks[i], f);
}

int is_binop(InterCode *code)
{
    if (code->kind == ADD || code->kind == SUB || code->kind == DIV || code->kind == MUL)
        return 1;
    else
        return 0;
}

int has_op2(InterCode *code)
{
    if (code->kind == ADD || code->kind == SUB || code->kind == DIV || code->kind == MUL || code->kind == ADDR_ASSIGN)
    {
        if (code->u.binop.op2 != NULL)
        {
            return 1;
        }
    }
    return 0;
}

int is_address_assign(InterCode *code)
{
}

int count_ope(int v_index, InterCodes *c)
{
    int count = 0;
    InterCodes *head = c;
    while (head != NULL && head->code != NULL)
    {
        InterCode *code = head->code;
        if (is_binop(code) == 1)
        {
            if (code->u.binop.op1->kind != CONSTANT && code->u.binop.op1->u.var_no == v_index)
                count++;
            if (code->u.binop.result->kind != CONSTANT && code->u.binop.result->u.var_no == v_index)
                count++;
        }
        if (code->kind == ASSIGN)
        {
        }
    }
}

BB *find_BB_using_label(int label)
{
    for (int i = 0; i < block_size; i++)
    {
        if (blocks[i]->start->code->kind == LABEL && blocks[i]->start->code->u.label_index == label)
        {
            return blocks[i];
        }
    }
}

void get_BB()
{
    InterCodes *head = codes;
    int size = 0;
    while (head != NULL)
    {
        head = head->next;
        size++;
    }
    head = codes->next;
    int i = 1;
    while (head != NULL && head->code != NULL)
    {
        head->code->lineno = i++;
        head = head->next;
    }
    head = codes->next;
    blocks = malloc(sizeof(BB *) * size);
    leaders = malloc(sizeof(InterCodes *) * size);
    if_go = malloc(sizeof(int) * var_size);
    for (int i = 0; i < var_size; i++)
        if_go[i] = 0;
    size = 0;
    InterCodes *leader = codes->next;
    leaders[size++] = leader;
    // leader->code->lineno = 1;
    head = leader->next;
    constant_size = 0;
    while (head != NULL && head->code != NULL)
    {
        InterCode *code = head->code;
        if (is_binop(code))
        {
            if (code->u.binop.op2->kind == CONSTANT)
                constant_size++;
        }
        if (code->kind == ASSIGN)
        {
            if (code->u.assign.right->kind == CONSTANT)
                constant_size++;
        }
        if (code->kind == GOTO || code->kind == IF_GOTO)
        {
            InterCodes *label;
            if (code->kind == IF_GOTO)
            {
                // if_go[code->u.if_go.]
                if (code->u.if_go.op1->kind != CONSTANT)
                {
                    if_go[code->u.if_go.op1->u.var_no] = 1;
                }
                if (code->u.if_go.op2->kind != CONSTANT)
                {
                    if_go[code->u.if_go.op2->u.var_no] = 1;
                }
                label = find_label(code->u.if_go.label_index);
            }
            if (code->kind == GOTO)
            {
                label = find_label(code->u.label_index);
            }
            int ret = insert_leaders(label, size);
            if (ret == 0)
                size++;
            // leaders[size++] = label;
            InterCodes *n = head->next;
            if (n != NULL && n->code != NULL)
            {
                // print_code(n->code, stdout);
                int ret = insert_leaders(n, size);
                if (ret == 0)
                    size++;
            }
        }
        head = head->next;
    }
    leader_size = size;
    constant_size *= 3;
    constant_dag = malloc(sizeof(DAGNode *) * constant_size);
    for (int i = 0; i < constant_size; i++)
        constant_dag[i] = NULL;
    i = 0;
    head = codes->next->next;
    blocks[i] = malloc(sizeof(BB));
    blocks[i]->start = leaders[0];
    i += 1;
    int b_size = 1;
    while (head != NULL && head->code != NULL)
    {
        b_size++;
        if (is_leaders(head, leader_size) == 0)
        {
            // printf("enter here ");
            // print_code(head->code, stdout);
            blocks[i - 1]->end = head->prev;
            blocks[i - 1]->size = b_size;
            b_size = 1;
            blocks[i++] = malloc(sizeof(BB));
            blocks[i - 1]->start = head;
            // printf("leave here\n");
        }
        if (head->next == NULL)
            break;
        head = head->next;
    }
    if (head->code == NULL)
        head = head->prev;
    blocks[i - 1]->end = head;
    block_size = i;
    cfg = malloc(sizeof(CFGNode) * block_size);
    for (int i = 0; i < block_size; i++)
        cfg[i].in = malloc(sizeof(BB *) * 3);
    //void build_cfg()
    for (int i = 0; i < block_size; i++)
    {
        if (blocks[i]->end->code->kind == GOTO)
        {
            cfg[i].size = 1;
            cfg[i].in[0] = find_BB_using_label(blocks[i]->end->code->u.label_index);
        }
        else if (blocks[i]->end->code->kind == IF_GOTO)
        {
            cfg[i].size = 1;
            cfg[i].in[0] = find_BB_using_label(blocks[i]->end->code->u.if_go.label_index);
            if (i != block_size - 1)
            {
                cfg[i].size = 2;
                cfg[i].in[1] = blocks[i + 1];
            }
        }
        else
        {
            if (i != block_size - 1)
            {
                cfg[i].size = 1;
                cfg[i].in[0] = blocks[i + 1];
            }
            else
            {
                cfg[i].size = 0;
            }
        }
    }
    // free(cfg);
}

int assign_optimize_block(BB *block)
{
    int flag = 0;
    InterCodes *prev = block->start;
    while (prev != block->end)
    {
        InterCodes *next = prev->next;
        while (1)
        {
            if (next->code->kind == SPACE)
                next = next->next;
            else
                break;
        }
        if (is_binop(prev->code) == 1 && next->code->kind == ASSIGN)
        {
            // if (prev->code->u.binop.result->u.var_no == next->code->u.assign.right->u.var_no && if_go[prev->code->u.binop.result->u.var_no] != 1)
            if (prev->code->u.binop.result->u.var_no == next->code->u.assign.right->u.var_no &&
                (next->next->code->live_var[prev->code->u.binop.result->u.var_no] == 0 || next->next->code->live_var[prev->code->u.binop.result->u.var_no] == -1) &&
                if_go[prev->code->u.binop.result->u.var_no] != 1)
            {
                flag = 1;
                prev->code->u.binop.result = next->code->u.assign.left;
                next->code->kind = SPACE;
                // prev->next = next->next;
                // next->next->prev = prev;
                // if (next == block->end)
                // {
                //     block->end = prev;
                //     break;
                // }
            }
        }
        // if (prev->code->kind == ASSIGN && next->code->kind == ASSIGN && (next->next->code->live_var[prev->code->u.binop.result->u.var_no] == 0 || next->next->code->live_var[prev->code->u.binop.result->u.var_no] == -1) && if_go[prev->code->u.binop.result->u.var_no] != 1)
        if (prev->code->kind == ASSIGN && next->code->kind == ASSIGN && (next->next->code->live_var[prev->code->u.assign.left->u.var_no] == 0 || next->next->code->live_var[prev->code->u.assign.left->u.var_no] == -1) && if_go[prev->code->u.assign.left->u.var_no] != 1)
        {
            Operand *left = prev->code->u.assign.left;
            Operand *right = next->code->u.assign.right;
            if (left->kind == right->kind && left->u.var_no == right->u.var_no)
            {
                flag = 1;
                prev->code->u.assign.left = next->code->u.assign.left;
                next->code->kind = SPACE;
                // prev->next = next->next;
                // next->next->prev = prev;
                // if (next == block->end)
                // {
                //     block->end = prev;
                //     break;
                // }
            }
        }
        if (prev->code->kind == ASSIGN && next->code->kind == ASSIGN)
        {
            Operand *o1, *o2, *o3, *o4;
            o1 = prev->code->u.assign.left;
            o2 = prev->code->u.assign.right;
            o3 = next->code->u.assign.left;
            o4 = next->code->u.assign.right;
            if (o1->kind != CONSTANT && o2->kind != CONSTANT && o3->kind != CONSTANT && o4->kind != CONSTANT)
            {
                int v1, v2, v3, v4;
                v1 = o1->u.var_no;
                v2 = o2->u.var_no;
                v3 = o3->u.var_no;
                v4 = o4->u.var_no;
                if (v1 == v4 && v2 == v3)
                {
                    flag = 1;
                    if (next->next->code->live_var[v1] == 0 || next->next->code->live_var[v1] == -1)
                    {
                        prev->code->kind = SPACE;
                    }
                    next->code->kind = SPACE;
                }
            }
        }
        if (prev->code->kind == ASSIGN)
        {
            Operand *left = prev->code->u.assign.left;
            Operand *right = prev->code->u.assign.right;
            if (left->kind == right->kind && left->kind != CONSTANT)
            {
                if (left->u.var_no == right->u.var_no)
                {
                    flag = 1;
                    prev->code->kind = SPACE;
                }
            }
        }
        prev = prev->next;
    }
    return flag;
}

void assign_optimize()
{
    for (int i = 0; i < block_size; i++)
    {
        int ret = 0;
        int t = 0;
        do
        {
            ret = assign_optimize_block(blocks[i]);
        } while (ret != 0);
    }
}

void live_var_analyze(BB *block)
{
    InterCodes *prev = block->end;
    while (1)
    {
        InterCode *code = prev->code;
        if (prev == block->end)
        {
            code->live_var = malloc(sizeof(int) * var_size);
            for (int i = 0; i < var_size; i++)
                code->live_var[i] = 0;
        }
        else
        {
            code->live_var = malloc(sizeof(int) * var_size);
            for (int i = 0; i < var_size; i++)
                code->live_var[i] = prev->next->code->live_var[i];
        }
        if (is_binop(prev->code) == 1 || prev->code->kind == ADDR_ASSIGN)
        {
            Operand *result = code->u.binop.result;
            Operand *op1 = code->u.binop.op1;
            Operand *op2 = code->u.binop.op2;
            if (result->kind != CONSTANT)
            {
                code->live_var[result->u.var_no] = -1;
            }
            if (op1->kind != CONSTANT)
            {
                code->live_var[op1->u.var_no] = code->lineno;
            }
            if (op2->kind != CONSTANT)
            {
                code->live_var[op2->u.var_no] = code->lineno;
            }
        }
        if (code->kind == ASSIGN || code->kind == ASSIGN_DEREFERENCE || code->kind == DEREFERENCE_ASSIGN)
        {
            Operand *left = code->u.assign.left;
            Operand *right = code->u.assign.right;
            if (left->kind != CONSTANT)
                code->live_var[left->u.var_no] = -1;
            if (right->kind != CONSTANT)
                code->live_var[right->u.var_no] = code->lineno;
        }
        if (code->kind == RETURN)
        {
            Operand *o = code->u.ret;
            if (o->kind != CONSTANT)
                code->live_var[o->u.var_no] = code->lineno;
        }
        if (code->kind == ARG)
        {
            Operand *o = code->u.arg.arg;
            if (o->kind != CONSTANT)
                code->live_var[o->u.var_no] = code->lineno;
        }
        if (code->kind == READ || code->kind == WRITE)
        {
            Operand *o = code->u.rw;
            if (o->kind != CONSTANT)
                code->live_var[o->u.var_no] = code->lineno;
        }
        // print_code(code, stdout);
        // for (int i = 0; i < var_size; i++)
        // {
        //     printf("%d %d ",i, prev->code->live_var[i]);
        // }
        // printf("\n");
        if (prev == block->start)
            break;
        prev = prev->prev;
    }
    // while (1)
    // {
    //     for (int i = 0; i < max_value; i++)
    //     {
    //         printf("%d ", prev->code->live_var[i]);
    //     }
    //     printf("\n");
    //     if (prev == block->end)
    //         break;
    //     prev = prev->next;
    // }
}

int find_constant_dag(int value)
{
    for (int i = 0; i < constant_size; i++)
    {
        if (constant_dag[i] != NULL)
        {
            if (constant_dag[i]->v_value == value)
                return i;
        }
    }
    return -1;
}

int find_var_dag(int v_index)
{
    if (var_dag[v_index] != NULL)
        return 0;
    return -1;
}

int get_free_constant_dag()
{
    for (int i = 0; i < constant_size; i++)
    {
        if (constant_dag[i] == NULL)
            return i;
    }
}

DAGNode *create_or_get_node(Operand *o, int lineno)
{
    if (o->kind == CONSTANT)
    {
        int have = find_constant_dag(o->u.value);
        int index = get_free_constant_dag();
        if (have == -1)
        {
            DAGNode *constant = malloc(sizeof(DAGNode));
            constant->lineno = lineno;
            constant->kind = DAG_CONSTANT;
            constant->v_value = o->u.value;
            constant_dag[index] = constant;
            return constant;
        }
        else
            return constant_dag[have];
    }
    else
    {
        int have = find_var_dag(o->u.var_no);
        if (have == -1)
        {
            DAGNode *var = malloc(sizeof(DAGNode));
            var->lineno = lineno;
            var->kind = DAG_VAR;
            var->v_index = o->u.var_no;
            var_dag[var->v_index] = var;
            return var;
        }
        else
            return var_dag[o->u.var_no];
    }
}
DAGNode *def_value(InterCode *code)
{
    // common_sub_expression_elimination(code);
    if (code->kind == ASSIGN)
    {
        // print_code(code, stdout);
        Operand *left = code->u.assign.left;
        Operand *right = code->u.assign.right;
        DAGNode *n = create_or_get_node(right, code->lineno);
        DAGNode *res;
        if (var_dag[left->u.var_no] != NULL)
        {
            res = var_dag[left->u.var_no];
        }
        else
        {
            res = malloc(sizeof(DAGNode));
        }
        res->kind = DAG_VAR;
        res->op_kind = code->kind;
        res->lineno = code->lineno;
        res->v_index = left->u.var_no;
        res->left = n;
        res->last_modify_line = code->lineno;
        var_dag[left->u.var_no] = res;
        // if (n->kind == DAG_VAR)
        //     printf("%d v_index:%d\n", left->u.var_no, n->v_index);
        // if (n->kind == DAG_CONSTANT)
        //     printf("%d v_value:%d\n", left->u.var_no, n->v_value);
    }
    else if (is_binop(code) == 1)
    {
        // print_code(code, stdout);
        Operand *result = code->u.binop.result;
        Operand *op1 = code->u.binop.op1;
        Operand *op2 = code->u.binop.op2;
        DAGNode *l = create_or_get_node(op1, code->lineno);
        DAGNode *r = create_or_get_node(op2, code->lineno);
        DAGNode *op;
        if (var_dag[result->u.var_no] != NULL)
        {
            op = var_dag[result->u.var_no];
        }
        else
        {
            op = malloc(sizeof(DAGNode));
        }
        op->kind = DAG_OP_WITH_VAR;
        op->op_kind = code->kind;
        op->lineno = code->lineno;
        op->left = l;
        op->right = r;
        op->v_index = result->u.var_no;
        var_dag[result->u.var_no] = op;
        // printf("%d\n", op->v_value]->v_index);
    }
}

void build_dag_node(BB *block)
{
    InterCodes *prev = block->start;
    while (1)
    {
        InterCode *code = prev->code;
        if (code->kind == ASSIGN && is_binop(code) == 1)
            def_value(code);
        if (prev == block->end)
            break;
        prev = prev->next;
    }
}

void build_dag()
{
    for (int i = 0; i < block_size; i++)
    {
        live_var_analyze(blocks[i]);
    }
    // for (int i = 0; i < block_size; i++)
    // {
    //     aes = malloc(sizeof(AE));
    //     var_dag = malloc(sizeof(DAGNode *) * var_size);
    //     build_dag_node(blocks[i]);
    //     // common_sub_expression_elimination();
    //     // dead_code_optimize_block();
    // }
}

int constant_optimize_block(BB *block)
{
    int flag = 0;
    InterCodes *prev = block->start;
    int vs[var_size];
    for (int i = 0; i < var_size; i++)
        vs[i] = -1;
    while (prev != block->end)
    {
        InterCodes *next = prev->next;
        if (is_binop(prev->code) == 1 && has_op2(next->code) == 1)
        {
            Operand *left = prev->code->u.binop.op1;
            Operand *right = prev->code->u.binop.op2;
            Operand *result = prev->code->u.binop.result;
            if (left->kind == CONSTANT && right->kind == CONSTANT)
            {
                if (prev->code->kind == ADD)
                    vs[result->u.var_no] = left->u.value + right->u.value;
                if (prev->code->kind == SUB)
                    vs[result->u.var_no] = left->u.value - right->u.value;
                if (prev->code->kind == DIV)
                    vs[result->u.var_no] = left->u.value / right->u.value;
                if (prev->code->kind == MUL)
                    vs[result->u.var_no] = left->u.value * right->u.value;
                left = next->code->u.binop.op1;
                right = next->code->u.binop.op2;
                if (right->kind != CONSTANT && right->u.var_no == result->u.var_no)
                {
                    if (next->next->code->live_var[result->u.var_no] == 0)
                    {
                        Operand *o = malloc(sizeof(Operand));
                        o->kind = CONSTANT;
                        o->u.value = vs[result->u.var_no];
                        next->code->u.binop.op2 = o;
                        prev->code->kind = SPACE;
                        flag = 1;
                    }
                }
                if (left->kind != CONSTANT && left->u.var_no == result->u.var_no)
                {
                    if (next->next->code->live_var[result->u.var_no] == 0)
                    {
                        Operand *o = malloc(sizeof(Operand));
                        o->kind = CONSTANT;
                        o->u.value = vs[result->u.var_no];
                        next->code->u.binop.op1 = o;
                        prev->code->kind = SPACE;
                        flag = 1;
                    }
                }
            }
        }
        prev = prev->next;
    }
    return flag;
}

void constant_optimize()
{
    for (int i = 0; i < block_size; i++)
    {
        // int ret = constant_optimize_block(blocks[i]);
        int ret = 0;
        do
        {
            ret = constant_optimize_block(blocks[i]);
        } while (ret != 0);
    }
}

int dead_code_optimize_block(BB *block)
{
    InterCodes *prev = block->start;
    while (prev != block->end)
    {
        int *live_var = prev->code->live_var;
        InterCodes *next = prev->next;
        if (is_binop(prev->code) == 1 || prev->code->kind == ASSIGN)
        {
            if (prev->code->kind == ASSIGN)
            {
                int var_no = prev->code->u.assign.left->u.var_no;
                // for(int i = 0;i<max_value;i++) printf("%d ", next->code->live_var[i]);
                // printf("\n");
                if ((next->code->live_var[var_no] == 0 || next->code->live_var[var_no] == -1) && if_go[var_no] != 1)
                {
                    // print_code(prev->code, stdout);
                    // print_code(next->code, stdout);
                    // printf("\n");
                    // int var_no = prev->code->u.assign.left->u.var_no;
                    // prev->code->kind=SPACE;
                }
            }
            if (is_binop(prev->code) == 1)
            {
                int var_no = prev->code->u.binop.result->u.var_no;
                if ((next->code->live_var[var_no] == 0 || next->code->live_var[var_no] == -1) && if_go[var_no] != 1)
                {
                    // print_code(prev->code, stdout);
                    // print_code(next->code, stdout);
                    // printf("\n");
                    // int var_no = prev->code->u.assign.left->u.var_no;
                    // prev->code->kind=SPACE;
                }
            }
        }
        prev = prev->next;
    }
    return 0;
}

void dead_code_optimize()
{
    for (int i = 0; i < block_size; i++)
    {
        int ret = 0;
        do
        {
            ret = dead_code_optimize_block(blocks[i]);
        } while (ret != 0);
    }
}

void insert_aes(InterCode *code)
{
    next->lineno = code->lineno;
    next->op_kind = code->kind;
    next->next = malloc(sizeof(next));
    next->next->lineno = -1;
    if (is_binop(code) == 1)
    {
        // print_code(code, stdout);
        next->res = code->u.binop.result;
        next->op1 = code->u.binop.op1;
        next->op2 = code->u.binop.op2;
    }
    next->next->prev = next;
    next = next->next;
    next->next = NULL;
}

/**
 * 1. 扫描每个语句 
 *  1） 如果发现一条赋值语句，如果这个变量
 *      1） 后续没有赋值语句且在其他block中出现，不能删除
 *      2） 其他block没有这个变量或者后续有赋值语句，直接删除，中间全部替换
 *  2） 如果发现一条binop语句
 *      1） 查找整个AES，如果没有找到表达式相同的语句，直接插入
 *      2） 如果找到了表达式相同的语句
 *          1） 后续没有赋值语句且在其他block中出现，不能删除
 *          2） 其他block没有这个变量或者后续有赋值语句，直接删除，中间全部替换
 *      3） 如果binop语句的res是其他aes中的参数，那么直接删除其他的aes，并且对于每个被删除的aes，如果他们的res被其他变量引用了，就全都kill掉
 *  3） 
*/
AE *find_aes(InterCode *code)
{
    AE *find = aes->next;
    // if (code->kind == ASSIGN)
    // {
    //     while (1)
    //     {

    //         if (find->next == NULL)
    //             break;
    //         find = find->next;
    //     }
    // }
    if (is_binop(code) == 1)
    {
        while (1)
        {
            if (find->lineno == -1)
            {
                break;
            }
            Operand *o1 = code->u.binop.op1;
            Operand *o2 = code->u.binop.op2;
            if (o1->u.var_no == find->op1->u.var_no && o2->kind == find->op2->kind && o2->u.var_no == find->op2->u.var_no && find->op_kind == code->kind)
            {
                // print_code(code, stdout);
                return find;
            }
            if (find->next == NULL)
                break;
            find = find->next;
        }
    }
    return NULL;
}

int common_sub_expression_elimination_block(BB *block)
{
    InterCodes *prev = block->start;
    int ret = 0;
    while (1)
    {
        InterCode *code = prev->code;
        InterCodes *bl = prev->next;
        if(prev->next==NULL) break;
        while (1)
        {
            if (bl->code->kind == SPACE)
                bl = bl->next;
            else
                break;
        }
        if (code->kind == ASSIGN)
        {
            int result = -1;
            while (1)
            {
                if (bl->code->kind == ASSIGN)
                {
                    if (bl->code->u.assign.left->u.var_no == code->u.assign.left->u.var_no)
                    {
                        result = bl->code->lineno;
                    }
                }
                if (is_binop(bl->code) == 1)
                {
                    if (bl->code->u.binop.result->u.var_no == code->u.assign.left->u.var_no)
                    {
                        result = bl->code->lineno;
                    }
                }
                if (result != 1)
                    break;
                if (bl->next == NULL)
                    break;
                bl = bl->next;
            }
            if (result != -1)
            {
                bl = prev->next;
                while (bl->code->lineno != result)
                {
                    if (bl->code->kind == ASSIGN)
                    {
                        if (bl->code->u.assign.right->u.var_no == code->u.assign.left->u.var_no)
                        {
                            bl->code->u.assign.right = code->u.assign.left;
                            // print_code(bl->code, stdout);
                        }
                    }
                    if (is_binop(bl->code) == 1)
                    {
                        if (bl->code->u.binop.op1->u.var_no == code->u.assign.left->u.var_no)
                        {
                            bl->code->u.binop.op1 = code->u.assign.left;
                            // print_code(bl->code, stdout);
                        }
                        if (bl->code->u.binop.op2->kind != CONSTANT && bl->code->u.binop.op2->u.var_no == code->u.assign.left->u.var_no)
                        {
                            bl->code->u.binop.op2 = code->u.assign.left;
                            // print_code(bl->code, stdout);
                        }
                    }
                    bl = bl->next;
                }
            }
        }
        else if (is_binop(code) == 1)
        {
            // print_code(code, stdout);
            // printf(" \n");
            AE *ae = find_aes(code);
            int result = -1;
            if (ae == NULL)
            {
                insert_aes(code);
            }
            else
            {
                while (1)
                {
                    if (bl->code->kind == ASSIGN)
                    {
                        if (bl->code->u.assign.left->u.var_no == code->u.binop.result->u.var_no)
                        {
                            result = bl->code->lineno;
                        }
                    }
                    if (is_binop(bl->code) == 1)
                    {
                        if (bl->code->u.binop.result->u.var_no == code->u.binop.result->u.var_no)
                        {
                            result = bl->code->lineno;
                        }
                    }
                    if (result != 1)
                        break;
                    if (bl->next == NULL)
                        break;
                    bl = bl->next;
                }
                if (result != -1)
                {
                    bl = prev->next;
                    while (bl->code->lineno != result)
                    {
                        if (bl->code->kind == ASSIGN)
                        {
                            if (bl->code->u.assign.right->u.var_no == code->u.binop.result->u.var_no)
                            {
                                bl->code->u.assign.right = code->u.binop.result;
                                // print_code(bl->code, stdout);
                            }
                        }
                        if (is_binop(bl->code) == 1)
                        {
                            if (bl->code->u.binop.op1->u.var_no == code->u.binop.result->u.var_no)
                            {
                                bl->code->u.binop.op1 = code->u.binop.result;
                                // print_code(bl->code, stdout);
                            }
                            if (bl->code->u.binop.op2->kind != CONSTANT && bl->code->u.binop.op2->u.var_no == code->u.binop.result->u.var_no)
                            {
                                bl->code->u.binop.op2 = code->u.binop.result;
                                // print_code(bl->code, stdout);
                            }
                        }
                        bl = bl->next;
                    }
                }
            }
        }
        if (prev == block->end)
            break;
        prev = prev->next;
    }
    return 0;
}

// 如果一个value只出现在一个bb里面，那么如果他被赋值成了另一个value，则可以直接替换，省略这条赋值语句，直到下一次被赋值为止
void common_sub_expression_elimination()
{
    for (int i = 0; i < block_size; i++)
    {
        int ret = 0;
        do
        {
            aes = malloc(sizeof(AE));
            next = malloc(sizeof(AE));
            next->next = NULL;
            next->prev = aes;
            aes->next = next;
            aes->next->lineno = -1;
            ret = common_sub_expression_elimination_block(blocks[i]);
        } while (ret != 0);
    }
}

void optimize(int v_index, char *file)
{
    var_size = v_index;
    get_BB();
    // print_blocks(stdout);
    build_dag();
    constant_optimize();
    assign_optimize();
    // common_sub_expression_elimination();
    dead_code_optimize();
    // FILE *f = fopen("opt.ir", "w+");
    FILE *f = fopen(file, "w+");
    print_blocks(f);
    fclose(f);
}