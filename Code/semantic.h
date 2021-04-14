#include "global.h"

#define SEMANTIC_DEBUG 0
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
#define DefList_DefDefList 1
#define Def_SpecifierDecListSEMI 1
#define DecList_Dec 1
#define DecList_DecCOMMADecList 2
#define Dec_VarDec 1
#define Dec_VarDecASSIGNOPExp 2
#define VarDec_ID 1
#define VarDec_VarDecLbIntRb 2
#define FunDec_IDLPVarListRP 1
#define FunDec_IDLPRP 2
#define VarList_ParamDecCOMMAVarList 1
#define VarList_ParamDec 2
#define ParamDec_SpecifierVarDec 1
#define StmtList_StmtStmtList 1
#define Stmt_ExpSEMI 1
#define Stmt_CompSt 2
#define Stmt_ReturnExpSEMI 3
#define Stmt_IfLpExpRpStmt 4
#define Stmt_IfLpExpRpStmtElseStmt 5
#define Stmt_WhileLpExpRpStmt 6
#define Exp_ExpAssignopExp 1
#define Exp_ExpAndExp 2
#define Exp_ExpOrExp 3
#define Exp_ExpRelopExp 4
#define Exp_ExpPlusExp 5
#define Exp_ExpMinusExp 6
#define Exp_ExpStarExp 7
#define Exp_ExpDivExp 8
#define Exp_LpExpRp 9
#define Exp_MinusExp 10
#define Exp_NotExp 11
#define Exp_IdLpArgsRp 12
#define Exp_IdLpRp 13
#define Exp_ExpLbExpRb 14
#define Exp_ExpDotId 15
#define Exp_Id 16
#define Exp_Int 17
#define Exp_Float 18
// | Exp AND Exp {$$=buildAST("Exp",3,$1,$2,$3,2);}
// | Exp OR Exp {$$=buildAST("Exp",3,$1,$2,$3,3);}
// | Exp RELOP Exp {$$=buildAST("Exp",3,$1,$2,$3,4);}
// | Exp PLUS Exp {$$=buildAST("Exp",3,$1,$2,$3,5);}
// | Exp MINUS Exp {$$=buildAST("Exp",3,$1,$2,$3,6);}
// | Exp STAR Exp {$$=buildAST("Exp",3,$1,$2,$3,7);}
// | Exp DIV Exp {$$=buildAST("Exp",3,$1,$2,$3,8);}
// | LP Exp RP {$$=buildAST("Exp",3,$1,$2,$3,9);}
// | MINUS Exp {$$=buildAST("Exp",2,$1,$2,10);}
// | NOT Exp {$$=buildAST("Exp",2,$1,$2,11);}
// | ID LP Args RP {$$=buildAST("Exp",4,$1,$2,$3,$4,12);}
// | ID LP RP {$$=buildAST("Exp",3,$1,$2,$3,13);}
// | Exp LB Exp RB {$$=buildAST("Exp",4,$1,$2,$3,$4,14);}
// | Exp DOT ID {$$=buildAST("Exp",3,$1,$2,$3,15);}
// | ID {$$=buildAST("Exp",1,$1,16);}
// | INT {$$=buildAST("Exp",1,$1,17);}
// | FLOAT {$$=buildAST("Exp",1,$1,18);}
#define ANONYMITY "!@#$^&&"

typedef struct Type Type;
typedef struct Field Field;
typedef struct Symbol Symbol;
typedef struct Array Array;
typedef enum TypeKind TypeKind;
typedef enum TypeValue TypeValue;
typedef enum BasicType BasicType;
typedef enum SymbolKind SymbolKind;
enum TypeKind
{
    BASIC,
    ARRAY,
    STRUCTURE,
    UNDEF,
    DEF
};

enum TypeValue
{
    LEFT,
    RIGHT
};

enum BasicType
{
    INT,
    FLOAT
};

struct Array
{
    Type *type;
    int size;
};

struct Type
{
    TypeKind kind;
    TypeValue value;
    union
    {
        BasicType basic;
        Array *array;
        Field *field;
    };
};

struct Field
{
    Type *type;
    char *name;
    Field *next;
};

enum SymbolKind
{
    VAR,
    FIELD,
    STRUCT,
    FUNCTION,
    FUNCTION_PARAM
};

struct Symbol
{
    SymbolKind kind;
    char *name;
    Type *type;
    int hashcode;
    char *value;
    Symbol *next;
};

Symbol **symbol_table;

int semanticAnalyze(int);                             //done
int ProgramAnalyze(int);                              //done
int ExtDefListAnalyze(int);                           //done
int ExtDefAnalyze(int);                               //done
int SpecifierAnalyze(int, Type *);                    //done
int ExtDecListAnalyze(int, Type *);                   //done
int FunDecAnalyze(int index, Type *type, TypeKind);   //done
int CompStAnalyze(int);                               //done
int VarDecAnalyze(int, Type *, Field *, SymbolKind);  //done
int StructAnalyze(int, Type *);                       //done
char *OptTagAnalyze(int);                             //done
int DefListAnalyze(int, Field *, SymbolKind);         //done
int DefAnalyze(int, Field *, SymbolKind);             //done
char *TagAnalyze(int);                                //done
Field *VarListAnalyze(int);                           //done
Field *ParamDecAnalyze(int);                          //done
int StmtListAnalyze(int);                             //done
Type *ExpAnalyze(int);                                //done
int StmtAnalyze(int);                                 //done
int DecListAnalyze(int, Type *, Field *, SymbolKind); //done
int DecAnalyze(int, Type *, Field *, SymbolKind);     //done
Field *ArgsAnalyze(int);                              //done
int IntAnalyze(int);