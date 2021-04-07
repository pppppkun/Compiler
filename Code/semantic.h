#include "global.h"

#define ExtDefList_ExtDefExtDefList 1
#define ExtDef_SpecifierExtDecListSEMI 1
#define ExtDef_SpecifierSEMI 2
#define ExtDef_SpecifierFunDecSEMI 3
#define ExtDef_SpecifierFunDecCompSt 4
#define ExtDecList_VarDec 1
#define ExtDecList_VarDecCOMMAExtDecList 2
#define Specifier_TYPE 1
#define Specifier_StructSpecifier 2
#define StructSpecifier_STRUCTOptTagLCDefListRC 1
#define StructSpecifier_STRUCTTag 2
#define OptTag_ID 1
#define Tag_ID 1
#define VarDec_ID 1
#define VarDec_LBINTRB 2
#define FunDec_IDLPVarListRP 1
#define FunDec_IDLPRP 2

int semanticAnalyze(int);
int programAnalyze(int);
int ExtDefListAnalyze(int);
int ExtDefAnalyze(int);
int SpecifierAnalyze(int);
int ExtDecListAnalyze(int);
int FunDecAnalyze(int);
int CompStAnalyze(int);
int VarDecAnalyze(int);
int StructAnalyze(int);
int OptTagAnalyze(int);
int DefListAnalyze(int);
int TagAnalyze(int);
int VarListAnalyze(int);
int ParamDecAnalyze(int);
int DefListAnalyze(int);
int StmtListAnalyze(int);
int ExpAnalyze(int);
int StmtAnalyze(int);
int DefAnalyze(int);
int DecListAnalyze(int);
int DecAnalyze(int);
int ArgsAnalyze(int);