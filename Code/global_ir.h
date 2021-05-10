#ifndef _IRGLOBAL_
#define _IRGLOBAL_

#define IR_DEBUG 1
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
#define Args_ExpCommaArgs 1
#define Args_Exp 2
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
    char *struct_name;
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
    int lineno;
    char *value;
    Symbol *next;
};

Symbol **symbol_table;
int *function_index;


typedef struct Operand Operand;
typedef struct InterCode InterCode;
typedef struct InterCodes InterCodes;

struct Operand {
    enum {VARIABLE, CONSTANT, ADDRESS} kind;
    union {
        int var_no;
        int value;
    } u;
};

struct InterCode {
    enum {LABEL, IR_FUNCTION, ASSIGN, ADD, SUB, MUL, DIV, GOTO, IF_GOTO, RETURN, DEC, ARG, CALL, PARAM, READ, WRITE} kind;
    union {
        char* function_name;
        struct {Operand * right, left;} assign;
        struct {Operand * result, op1, op2;} binop;
    }u;
};

struct InterCodes {
    InterCode * code;
    InterCodes * prev;
    InterCodes * next;
};

InterCodes * codes;
InterCodes * now;

#endif

#ifndef _UTILS_
#define _UTILS_

void DebugPrintNameType(ASTNode *node);
void DebugPrintNameValue(ASTNode *node);
int *GetSon(ASTNode *node);

#endif