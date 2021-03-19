%locations
%{
    #include "lex.yy.c"
    #include <stdio.h>
    int yyerror(char* msg);
%}

%union {
    int type_int;
    float type_float;
    char* type_string;
}

%token SEMI COMMA TYPE STRUCT LC RC ID LB INT RB LP RP RETURN IF ELSE WHILE
%token ASSIGNOP AND OR RELOP PLUS MINUS STAR DIV NOT FLOAT DOT

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT NEG
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

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
    | Specifier error SEMI
    | Specifier error RC
    | error SEMI
    ;
ExtDecList : VarDec
    | VarDec COMMA ExtDecList
    ;

/* Specifiers */
Specifier : TYPE
    | StructSpecifier
    ;
StructSpecifier : STRUCT OptTag LC DefList RC
    | STRUCT Tag
    | STRUCT error LC DefList RC
    | STRUCT error LC error RC
    | STRUCT error RC
    ;
OptTag : ID
    |
    ;
Tag : ID
    ;

/* Declarators */
VarDec : ID
    | VarDec LB INT RB
    | VarDec LB error RB
    ;
FunDec : ID LP VarList RP
    | ID LP RP
    | ID LP error RP
    | error LP VarList RP
    ;
VarList : ParamDec COMMA VarList
    | ParamDec
    ;
ParamDec : Specifier VarDec
    ;

/* Statements */
CompSt : LC DefList StmtList RC
    | error RC
    ;
StmtList : Stmt StmtList
    |
    ;
Stmt : Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    | RETURN error SEMI
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE
    | IF LP Exp RP error %prec LOWER_THAN_ELSE
    | IF LP error RP error %prec LOWER_THAN_ELSE
    | IF LP error RP Stmt ELSE Stmt
    | IF LP Exp RP error ELSE Stmt
    | IF LP Exp RP Stmt ELSE error
    | WHILE LP error RP Stmt
    ;

/* Local Definitions */
DefList : Def DefList
    |
    ;
Def : Specifier DecList SEMI
    | Specifier error SEMI
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
    | LP error RP
    | ID LP error RP
    | Exp LB error RB
    | Exp error Exp
    ;
Args : Exp COMMA Args
    | Exp
    | error COMMA Args
    ;

%%
int yyerror(char* msg){
    //printf("error: %s\n",msg);
    printf("Error type B at Line %d: %s near %s\n", yylineno, msg,yytext);
}