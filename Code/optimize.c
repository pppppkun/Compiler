#include "global.h"
#include "global_ir.h"

typedef struct BasicBlock BB;
typedef struct DAG DAG;
typedef struct DAGNode DAGNode;

struct BasicBlock
{
    InterCodes *start;
    InterCodes *end;
};

struct DAGNode
{
    // unique identifier
    int v_index;
    // constants
    int v_value;
    // operator symbol
    int op_kind;
    int result;
    DAGNode *left;
    DAGNode *right;
};

DAGNode **dag;
DAGNode **constant_dag;
InterCodes **leaders;

struct CFGNode
{
    BB *block;
    BB **in;
    BB **out;
};

int max_value;
int constant_size;
int block_size;

BB **blocks;

int is_leaders(InterCodes *c, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (leaders[i]->code->lineno == c->code->lineno)
        {
            return 0;
        }
    }
    return 1;
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

void get_BB()
{
    InterCodes *head = codes;
    int size = 0;
    while (head != NULL)
    {
        head = head->next;
        size++;
    }
    blocks = malloc(sizeof(BB *) * size);
    leaders = malloc(sizeof(InterCodes *) * size);
    size = 0;
    InterCodes *leader = codes->next;
    leaders[size++] = leader;
    leader->code->lineno = 1;
    head = leader->next;
    int i = 2;
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
        code->lineno = i++;
        if (code->kind == GOTO || code->kind == IF_GOTO)
        {
            InterCodes *label;
            if (code->kind == IF_GOTO)
            {
                label = find_label(code->u.if_go.label_index);
            }
            if (code->kind == GOTO)
            {
                label = find_label(code->u.label_index);
            }
            leaders[size++] = label;
            InterCodes *n = head->next;
            if (n != NULL && n->code != NULL)
            {
                leaders[size++] = head->next;
            }
        }
        head = head->next;
    }
    constant_dag = malloc(sizeof(DAGNode *) * constant_size);
    i = 0;
    head = codes->next->next;
    BB *block = malloc(sizeof(BB));
    block->start = leaders[0];
    blocks[i++] = block;
    while (head != NULL && head->code != NULL)
    {
        if (is_leaders(head, size) == 0)
        {
            block->end = head->prev;
            // print_block(block);
            block = malloc(sizeof(BB));
            block->start = head;
            blocks[i++] = block;
        }
        if (head->next == NULL)
            break;
        head = head->next;
    }
    if (head->code == NULL)
        head = head->prev;
    block->end = head;
    block_size = i;
}

int assign_optimize_block(BB *block)
{
    int flag = 0;
    InterCodes *prev = block->start;
    while (prev != block->end)
    {
        InterCodes *next = prev->next;
        if (is_binop(prev->code) == 1 && next->code->kind == ASSIGN)
        {
            if (prev->code->u.binop.result->u.var_no == next->code->u.assign.right->u.var_no)
            {
                flag = 1;
                prev->code->u.binop.result = next->code->u.assign.left;
                prev->next = next->next;
                next->next->prev = prev;
                if (next == block->end)
                {
                    block->end = prev;
                    break;
                }
            }
        }
        if (prev->code->kind == ASSIGN && next->code->kind == ASSIGN)
        {
            Operand *left = prev->code->u.assign.left;
            Operand *right = next->code->u.assign.right;
            if (left->kind == right->kind && left->u.var_no == right->u.var_no)
            {
                flag = 1;
                prev->code->u.assign.left = next->code->u.assign.left;
                prev->next = next->next;
                next->next->prev = prev;
                if (next == block->end)
                {
                    block->end = prev;
                    break;
                }
            }
        }
        if (prev->code->kind == ASSIGN)
        {
            Operand *left = prev->code->u.assign.left;
            Operand *right = prev->code->u.assign.right;
            if (left->kind == right->kind && left->kind != CONSTANT)
            {
                flag = 1;
                if (left->u.var_no == right->u.var_no)
                    prev->code->kind = SPACE;
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
            t += 1;
        } while (ret != 0 && t < 5);
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
            code->live_var = malloc(sizeof(int) * max_value);
            for (int i = 0; i < max_value; i++)
                code->live_var[i] = 0;
        }
        else
        {
            code->live_var = malloc(sizeof(int) * max_value);
            for (int i = 0; i < max_value; i++)
                code->live_var[i] = prev->next->code->live_var[i];
        }
        if (is_binop(prev->code) == 1)
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
        if (code->kind == ASSIGN)
        {
            Operand *left = code->u.assign.left;
            Operand *right = code->u.assign.right;
            if (left->kind != CONSTANT)
                code->live_var[left->u.var_no] = -1;
            if (right->kind != CONSTANT)
                code->live_var[right->u.var_no] = code->lineno;
        }
        // print_code(code, stdout);
        // for (int i = 0; i < max_value; i++)
        // {
        //     printf("%d ", prev->code->live_var[i]);
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

int get_free_constant_dag()
{
    for (int i = 0; i < constant_size; i++)
    {
        if (constant_dag[i] == NULL)
            return i;
    }
}

DAGNode *def_value(InterCode *code)
{
    DAGNode *ndag = malloc(sizeof(DAGNode));
    ndag->op_kind = code->kind;
    if (code->kind == ASSIGN)
    {
        Operand *left = code->u.assign.left;
        Operand *right = code->u.assign.right;
        if (right->kind == CONSTANT)
        {
            int have = find_constant_dag(right->u.value);
            int index = get_free_constant_dag();
            if (have == -1)
            {
                DAGNode *constant = malloc(sizeof(DAGNode));
                constant->v_value = right->u.value;
                constant_dag[index] = constant;
            }
        }
    }
    else
    {
        Operand *result = code->u.binop.result;
        Operand *op1 = code->u.binop.op1;
        Operand *op2 = code->u.binop.op2;
    }
}

void build_dag_node(BB *block)
{
    InterCodes *prev = block->start;
    while (1)
    {
        InterCode *code = prev->code;
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
    // dag = malloc(sizeof(DAGNode *) * max_value);
    // build_dag_node(blocks[0]);
}

int constant_optimize_block(BB *block)
{
    int flag = 0;
    InterCodes *prev = block->start;
    int vs[max_value];
    for (int i = 0; i < max_value; i++)
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

void optimize(int v_index, char *file)
{
    max_value = v_index;
    get_BB();
    build_dag();
    constant_optimize();
    assign_optimize();
    FILE *f = fopen(file, "w+");
    print_blocks(f);
    fclose(f);
}