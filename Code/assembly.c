#include "global.h"
#include "global_ir.h"

typedef struct assembly assembly;
typedef struct val val;

struct assembly
{
    char *code;
    assembly *next;
    assembly *prev;
};

struct val
{
    int v;
    val *next;
};

assembly *s_codes;
assembly *data;
assembly *s_head;
val *vals;
int regs = 0;

int get_reg()
{
    if (regs % 8 == 7)
    {
        regs = 0;
        return 7;
    }
    else
    {
        return regs++;
    }
}

int alloc_var(Operand *o)
{
    if (o->kind != CONSTANT)
    {
        val *v = vals;
        int find = 0;
        int x = o->u.var_no;
        while (v != NULL)
        {
            if (v->v == x)
            {
                find = 1;
            }
            v = v->next;
        }
        if (find == 0)
        {
            val *nv = malloc(sizeof(val));
            nv->v = x;
            nv->next = vals;
            vals = nv;
            assembly *nval = malloc(sizeof(assembly));
            nval->code = malloc(sizeof(char) * 64);
            sprintf(nval->code, "v%d: .word 0\n", x);
            nval->prev = data;
            nval->next = data->next;
            data->next = nval;
            nval->next->prev = nval;
        }
    }
}

int reg(Operand *o)
{
    if (o->kind == VARIABLE)
    {
        int x = o->u.var_no;
        s_head->next = malloc(sizeof(assembly));
        s_head->next->code = malloc(sizeof(char) * 64);
        int t = get_reg();
        sprintf(s_head->next->code, "  la $t%d, v%d\n  lw $t%d, 0($t%d)\n", t, x, t, t);
        s_head->next->prev = s_head;
        s_head = s_head->next;
        s_head->next = NULL;
        return t;
    }
    else if (o->kind == CONSTANT)
    {
        int t = get_reg();
        s_head->next = malloc(sizeof(assembly));
        s_head->next->code = malloc(sizeof(char) * 64);
        sprintf(s_head->next->code, "  li $t%d, %d\n", t, o->u.value);
        s_head->next->prev = s_head;
        s_head = s_head->next;
        s_head->next = NULL;
        return t;
    }
    else if (o->kind == ADDRESS)
    {
        int x = o->u.var_no;
        s_head->next = malloc(sizeof(assembly));
        s_head->next->code = malloc(sizeof(char) * 64);
        int t = get_reg();
        sprintf(s_head->next->code, "  la $t%d, v%d\n", t, x);
        s_head->next->prev = s_head;
        s_head = s_head->next;
        s_head->next = NULL;
        return t;
    }
}

int save(Operand *o, int reg)
{
    if (o->kind == VARIABLE || o->kind == ADDRESS)
    {
        int x = o->u.var_no;
        int t = get_reg();
        s_head->next = malloc(sizeof(assembly));
        s_head->next->code = malloc(sizeof(char) * 64);
        sprintf(s_head->next->code,
                "  la $t%d, v%d\n  sw $t%d, 0($t%d)\n",
                t, x, reg, t);
        s_head->next->prev = s_head;
        s_head = s_head->next;
        s_head->next = NULL;
    }
}

int toMIPS32(char *filename)
{
    FILE *f = fopen(filename, "w+");
    vals = malloc(sizeof(val));
    vals->next = NULL;
    vals->v = -1;
    s_codes = malloc(sizeof(assembly));
    s_codes->code = ".data\n";
    data = s_codes;
    s_codes->next = malloc(sizeof(assembly));
    s_codes->next->code = "_prompt: .asciiz \"Enter an integer:\"\n";
    s_codes->next->prev = s_codes;
    s_codes->next->next = malloc(sizeof(assembly));
    s_codes->next->next->code = "_ret: .asciiz \"\\n\"\n";
    s_codes->next->next->prev = s_codes->next;
    s_codes->next->next->next = malloc(sizeof(assembly));
    assembly *next = s_codes->next->next->next;
    next->code = ".globl main\n";
    next->prev = s_codes->next->next;
    next->next = malloc(sizeof(assembly));
    s_head = next->next;
    s_head->code = ".text\n";
    s_head->next = malloc(sizeof(assembly));
    s_head->prev = next;
    s_head = s_head->next;
    next = s_head;
    s_head->code = "read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n\nwrite:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n";
    s_head->next = NULL;
    s_head->prev = next;
    InterCodes *head_ = codes->next;
    while (head_ != NULL && head_->code != NULL)
    {
        InterCode *code = head_->code;
        switch (code->kind)
        {
        case LABEL:
        {
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            sprintf(s_head->next->code, "%d:\n", code->u.label_index);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            break;
        }
        case IR_FUNCTION:
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            sprintf(s_head->next->code, "%s:\n", code->u.function_name);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            break;
        case ASSIGN:
        {
            alloc_var(code->u.assign.left);
            alloc_var(code->u.assign.right);
            int r1 = get_reg();
            int r2 = reg(code->u.assign.right);
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            //"  move $t%d, $t%d\n"
            //"  "
            sprintf(s_head->next->code, "  move $t%d, $t%d\n", r1, r2);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            save(code->u.assign.left, r1);
            break;
        }
        case ADD:
        {
            alloc_var(code->u.binop.result);
            alloc_var(code->u.binop.op1);
            alloc_var(code->u.binop.op2);
            int r1 = get_reg();
            int r2 = reg(code->u.binop.op1);
            int r3 = reg(code->u.binop.op2);
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            sprintf(s_head->next->code, "  add $t%d, $t%d, $t%d\n", r1, r2, r3);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            save(code->u.binop.result, r1);
            break;
        }
        case SUB:
        {
            alloc_var(code->u.binop.result);
            alloc_var(code->u.binop.op1);
            alloc_var(code->u.binop.op2);
            int r1 = get_reg();
            int r2 = reg(code->u.binop.op1);
            int r3 = reg(code->u.binop.op2);
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            sprintf(s_head->next->code, "  sub $t%d, $t%d, $t%d\n", r1, r2, r3);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            save(code->u.binop.result, r1);
            break;
        }
        case MUL:
        {
            alloc_var(code->u.binop.result);
            alloc_var(code->u.binop.op1);
            alloc_var(code->u.binop.op2);
            int r1 = get_reg();
            int r2 = reg(code->u.binop.op1);
            int r3 = reg(code->u.binop.op2);
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            sprintf(s_head->next->code, "  mul $t%d, $t%d, $t%d\n", r1, r2, r3);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            save(code->u.binop.result, r1);
            break;
        }
        case DIV:
        {
            alloc_var(code->u.binop.result);
            alloc_var(code->u.binop.op1);
            alloc_var(code->u.binop.op2);
            int r1 = get_reg();
            int r2 = reg(code->u.binop.op1);
            int r3 = reg(code->u.binop.op2);
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            sprintf(s_head->next->code, "  div $t%d, $t%d\n  mflo $t%d\n", r2, r3, r1);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            save(code->u.binop.result, r1);
            break;
        }
        case ADDR_ASSIGN:
        {
            Operand *left = code->u.binop.result;
            Operand *right = code->u.binop.op1;
            Operand *constant = code->u.binop.op2;
            alloc_var(left);
            alloc_var(right);
            int r1 = get_reg();
            int r2 = get_reg();
            int r3 = -1;
            if (constant != NULL)
            {
                r3 = reg(constant);
            }
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            if (r3 == -1)
            {
                sprintf(s_head->next->code, "  la $t%d, v%d\n  move $t%d, $t%d\n", r2, right->u.var_no, r1, r2);
            }
            else
            {
                sprintf(s_head->next->code, "  la $t%d, v%d\n  add $t%d, $t%d, $t%d\n", r2, right->u.var_no, r1, r2, r3);
            }
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            save(left, r1);
            break;
        }
        case ASSIGN_DEREFERENCE:
        {
            printf("assign_def\n");
            break;
        }
        case DEREFERENCE_ASSIGN:
        {
            Operand *left = code->u.assign.left;
            Operand *right = code->u.assign.right;
            int r2 = reg(right);
            // s_head->next = malloc(sizeof(assembly));
            // s_head->next->code = malloc(sizeof(char) * 64);
            // s_head->next->prev = s_head;
            // s_head = s_head->next;
            // s_head->next = NULL;
            save(left, r2);
            break;
        }
        case GOTO:
        {
            s_head->next = malloc(sizeof(assembly));
            s_head->next->code = malloc(sizeof(char) * 64);
            sprintf(s_head->next->code, "  j %d\n", code->u.label_index);
            s_head->next->prev = s_head;
            s_head = s_head->next;
            s_head->next = NULL;
            break;
        }
        case IF_GOTO:

            break;
        case RETURN:

            break;
        case DEC:
        {
            val *nv = malloc(sizeof(val));
            nv->v = code->u.dec.x->u.var_no;
            nv->next = vals;
            vals = nv;
            assembly *nval = malloc(sizeof(assembly));
            nval->code = malloc(sizeof(char) * 64);
            sprintf(nval->code, "v%d: .space %d\n", nv->v, code->u.dec.size * 4);
            nval->prev = data;
            nval->next = data->next;
            data->next = nval;
            nval->next->prev = nval;
            break;
        }
        case ARG:

            break;
        case PARAM:

            break;
        case CALL:

            break;
        case READ:

            break;
        case WRITE:

            break;
        default:
            break;
        }
        head_ = head_->next;
    }
    while (s_codes != NULL)
    {
        fprintf(f, "%s", s_codes->code);
        s_codes = s_codes->next;
    }
}