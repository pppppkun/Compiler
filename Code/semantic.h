#include "global.h"

#define SEMANTIC_DEBUG 1
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
#define Dec_VarDecASSIGNOP_Exp 2
#define VarDec_ID 1
#define VarDec_LBINTRB 2
#define FunDec_IDLPVarListRP 1
#define FunDec_IDLPRP 2
#define VarList_ParamDecCOMMAVarList 1
#define VarList_ParamDec 2
#define ParamDec_SpecifierVarDec 1
#define StmtList_StmtStmtList 1
#define Stmt_ExpSEMI 1
#define Stmt_CompSt 2
#define Stmt_RETURNExpSEMI 3
#define Stmt_IFLPExpRPStmt 4
#define Stmt_IFLPExpRPStmtELSEStmt 5
#define Stmt_WHILELPExpRPStmt 6
#define ANONYMITY "!@#$^&&**&*&*&*&*&*&*&*&"

typedef struct Type Type;
typedef struct Field Field;
typedef struct Symbol Symbol;
typedef struct FunParam FunParam;
typedef enum TypeKind TypeKind;
typedef enum TypeValue TypeValue;
typedef enum BasicType BasicType;
typedef enum SymbolKind SymbolKind;

enum TypeKind {
    BASIC,ARRAY,STRUCTURE
};

enum TypeValue{
    LEFT,RIGHT
};

enum BasicType{
    INT,FLOAT
};

struct FunParam
{
    /* data */
    Type* type;
    FunParam* fun_param;
};


struct Type{
    TypeKind kind;
    TypeValue value;
    union{
        BasicType basic;
        struct{
            Type* type;
            int size;
        }* array;
        Field* field;
    };
};

struct Field{
    Type* type;
    char* name;
    Field* next;
};

enum SymbolKind {
    VAR,
    FIELD,
    STRUCT,
    FUNCION
};

struct Symbol
{
    SymbolKind kind;
    char* name;
    Type* type;
    int hashcode;
    Symbol* next;
};

Symbol** symbol_table;

int semanticAnalyze(int);
int ProgramAnalyze(int);
int ExtDefListAnalyze(int);
int ExtDefAnalyze(int);
int SpecifierAnalyze(int, Type*);
int ExtDecListAnalyze(int, Type*);
int FunDecAnalyze(int);
int CompStAnalyze(int);
int VarDecAnalyze(int, Type*, Field*, SymbolKind);
int StructAnalyze(int, Type*);
char* OptTagAnalyze(int);
int DefListAnalyze(int, Field*, SymbolKind);
int DefAnalyze(int, Field*, SymbolKind);
char* TagAnalyze(int);
int VarListAnalyze(int);
int ParamDecAnalyze(int);
int StmtListAnalyze(int);
int ExpAnalyze(int);
int StmtAnalyze(int);
int DecListAnalyze(int, Type*, Field*, SymbolKind);
int DecAnalyze(int, Type*, Field*, SymbolKind);
int ArgsAnalyze(int);