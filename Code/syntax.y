%locations
%{
    #include "lex.yy.c"
    #include <stdio.h>
    int yyerror(char* msg);
%}

%union {
    int type_int;
    float type_float;
    double type_double;   
}

%token <type_int> INT
%token <type_float> FLOAT
%token ADD SUB MUL DIV
%type <type_double> Exp Factor Term

%%
/* High-level Definitions */
Program : ExtDefList
    ;
ExtDefList : ExtDef ExtDefList
    |
    ;
ExtDef : Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    ;
ExtDecList : VarDec
    | VarDec COMMA ExtDecList
    ;
/* Specifiers */
Specifier : TYPE
    | StructSpecifier
    ;
StructSpecifier : STRUCT OptTag LC DefList RC
    | STRUCT OptTag
    ;
OptTag : ID
    |
    ;
Tag : ID
    ;
/* Declarators */
VarDec : ID
    | VarDec LB INT RB
    ;
FunDec : ID LP VarList RP
    | ID LP RP
    ;
VarList : ParamDec COMMA VarList
    | ParamDec
    ;
ParamDec : Specifier VarDec
    ;
/* Statements */
CompSt : LC DefList StmtList RC
    ;
StmtList : Stmt StmtList
    |
    ;

/* Local Definitions */
DefList : Def DefList
    |
    ;
Def : Specifier DecList SEMI
    ;
DecList : Dec
    | Dec COMMA DecList
    ;
Dec : VarDec
    | VarDec ASSIGNOP Exp

/* Expressions */
Exp : Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    ;
Args : Exp COMMA Args
    | Exp
    ;

%%
int yyerror(char* msg){
    //printf("error: %s\n",msg);
    printf("Error type B at Line %d: %s\n", yylineno, msg);
}