#include "ir.h"

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

void insert_function(char *function_name)
{
    InterCode *code = malloc(sizeof(InterCode));
    code->kind = FUNCTION;
    code->u.function_name = function_name;
    insert_code(code);
    free(code);
}

void insert_param(char* function_name){
    Symbol * s = SymbolGet(function_name, FUNCTION);
    if(IR_DEBUG)
    {
        if(s!=NULL) printf("Get From Table Success!\n");
    }
}

void gen_ir(int last_node)
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
    translate_Program(last_node);
    codes = codes->next;
    if (IR_DEBUG)
    {
        printf("Test IR\n");
    }
    while (codes != NULL && codes->code != NULL)
    {
        printf("%d\n", codes->code->kind);
        codes = codes->next;
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
    ASTNode *fun_dec = nodes[index];
    DebugPrintNameType(fun_dec);
    int *sons = GetSon(fun_dec);
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
    ASTNode *comp_st = nodes[index];
    DebugPrintNameType(comp_st);
}

// void translate_VarList(int index)
// {
//     ASTNode *var_list = nodes[index];
//     DebugPrintNameType(var_list);
//     int *sons = GetSon(var_list);
//     switch (var_list->type)
//     {
//     case VarList_ParamDec:
//         translate_ParamDec(sons[0]);
//         break;
//     case VarList_ParamDecCOMMAVarList:
//         translate_ParamDec(sons[0]);
//         translate_VarList(sons[2]);
//         break;
//     default:
//         break;
//     }
// }
// void translate_ParamDec(int index)
// {
//     ASTNode *param_dec = nodes[index];
//     DebugPrintNameType(param_dec);
//     int *sons = GetSon(param_dec);
//     insert_param()
// }
void translate_StmtList(int);
void translate_Stmt(int);
void translate_DefList(int);
void translate_Def(int);
void translate_DecList(int);
void translate_Dec(int);
Operand *translate_VarDec(int);
Operand *translate_Exp(int);
void translate_Cond(int, int, int);
void translate_Args(int);
