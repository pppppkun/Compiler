#include "global.h"
#include "global_ir.h"
#ifndef _IR_
#define _IR_

void gen_ir(int);
void translate_Program (int);
void translate_ExtDefList (int);
void translate_ExtDef (int);
void translate_FunDec (int);
void translate_CompSt (int);
void translate_StmtList (int);
void translate_Stmt (int);
void translate_DefList (int);
void translate_Def (int);
void translate_DecList (int);
void translate_Dec (int);
Operand* translate_VarDec (int);
Operand* translate_Exp (int);
void translate_Cond (int, int, int);
void translate_Args (int);
#endif