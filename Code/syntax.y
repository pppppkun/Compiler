%locations
%{
    #include "lex.yy.c"
    #include <stdio.h>
    int yyerror(char* msg);
    int buildAST(char* name, int childSum, ...);
    int AST_PRINT_FLAG = 1;
%}


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
Program : ExtDefList {$$=buildAST("Program",1,$1);}
    ;
ExtDefList : ExtDef ExtDefList  {$$=buildAST("ExtDefList",2,$1,$2);}
    | {$$=buildAST("ExtDefList",-1);}
    ;
ExtDef : Specifier ExtDecList SEMI {$$=buildAST("ExtDef",3,$1,$2,$3);}
    | Specifier SEMI {$$=buildAST("ExtDef", 2, $1,$2);}
    | Specifier FunDec CompSt {$$=buildAST("ExtDef", 3,$1,$2,$3);}
    | Specifier error SEMI
    | Specifier error RC
    | error SEMI
    ;
ExtDecList : VarDec {$$=buildAST("ExtDecList", 1, $1);}
    | VarDec COMMA ExtDecList {$$=buildAST("ExtDecList", 3,$1,$2,$3);}
    ;

/* Specifiers */
Specifier : TYPE {$$=buildAST("Specifier", 1, $1);}
    | StructSpecifier {$$=buildAST("Specifier", 1, $1);}
    ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=buildAST("StructSpecifier",5,$1,$2,$3,$4,$5);}
    | STRUCT Tag {$$=buildAST("StructSpecifier",2,$1,$2);}
    | STRUCT error LC DefList RC
    | STRUCT error LC error RC
    | STRUCT error RC
    ;
OptTag : ID {$$=buildAST("OptTag",1,$1);}
    | {$$=buildAST("OptTag",-1);}
    ;
Tag : ID {$$=buildAST("Tag",1,$1);}
    ;

/* Declarators */
VarDec : ID {$$=buildAST("VarDec",1,$1);}
    | VarDec LB INT RB {$$=buildAST("VarDec",4,$1,$2,$3,$4);}
    | VarDec LB error RB
    ;
FunDec : ID LP VarList RP {$$=buildAST("FunDec",4,$1,$2,$3,$4);}
    | ID LP RP {$$=buildAST("FunDec",3,$1,$2,$3);}
    | ID LP error RP
    | error LP VarList RP
    ;
VarList : ParamDec COMMA VarList {$$=buildAST("VarList",3,$1,$2,$3);}
    | ParamDec {$$=buildAST("VarList", 1, $1);}
    ;
ParamDec : Specifier VarDec {$$=buildAST("ParamDec", 2,$1,$2);}
    ;

/* Statements */
CompSt : LC DefList StmtList RC {$$=buildAST("CompSt",4,$1,$2,$3,$4);}
    | error RC
    ;
StmtList : Stmt StmtList {$$=buildAST("StmtList", 2,$1,$2);}
    | {$$=buildAST("StmtList", -1);}
    ;
Stmt : Exp SEMI {$$=buildAST("Stmt",2,$1,$2);}
    | CompSt {$$=buildAST("Stmt",1,$1);}
    | RETURN Exp SEMI {$$=buildAST("Stmt",3,$1,$2,$3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=buildAST("Stmt",5,$1,$2,$3,$4,$5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$=buildAST("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
    | WHILE LP Exp RP Stmt {$$=buildAST("Stmt",5,$1,$2,$3,$4,$5);}
    | RETURN error SEMI
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE
    | IF LP Exp RP error %prec LOWER_THAN_ELSE
    | IF LP error RP error %prec LOWER_THAN_ELSE
    | IF LP error RP Stmt ELSE Stmt
    | IF LP Exp RP error ELSE Stmt
    | IF LP Exp RP Stmt ELSE error
    | WHILE LP error RP Stmt
    | error SEMI
    ;

/* Local Definitions */
DefList : Def DefList {$$=buildAST("DefList",2,$1,$2);}
    | {$$=buildAST("DefList", -1);}
    ;
Def : Specifier DecList SEMI {$$=buildAST("Def",3,$1,$2,$3);}
    | Specifier error SEMI 
    ;
DecList : Dec {$$=buildAST("DecList",1,$1);}
    | Dec COMMA DecList {$$=buildAST("DecList",3,$1,$2,$3);}
    ;
Dec : VarDec {$$=buildAST("Dec",1,$1);}
    | VarDec ASSIGNOP Exp {$$=buildAST("Dec",3,$1,$2,$3);}

/* Expressions */
Exp : Exp ASSIGNOP Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp AND Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp OR Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp RELOP Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp PLUS Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp MINUS Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp STAR Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp DIV Exp {$$=buildAST("Exp",3,$1,$2,$3);}
    | LP Exp RP {$$=buildAST("Exp",3,$1,$2,$3);}
    | MINUS Exp {$$=buildAST("Exp",2,$1,$2);}
    | NOT Exp {$$=buildAST("Exp",2,$1,$2);}
    | ID LP Args RP {$$=buildAST("Exp",4,$1,$2,$3,$4);}
    | ID LP RP {$$=buildAST("Exp",3,$1,$2,$3);}
    | Exp LB Exp RB {$$=buildAST("Exp",4,$1,$2,$3,$4);}
    | Exp DOT ID {$$=buildAST("Exp",3,$1,$2,$3);}
    | ID {$$=buildAST("Exp",1,$1);}
    | INT {$$=buildAST("Exp",1,$1);}
    | FLOAT {$$=buildAST("Exp",1,$1);}
    | LP error RP
    | ID LP error RP
    | Exp LB error RB
    | Exp ASSIGNOP error
    | Exp AND error
    | Exp OR error
    | Exp RELOP error
    | Exp PLUS error
    | Exp MINUS error
    | Exp STAR error
    | Exp DIV error
    | MINUS error
    | NOT error
    | error ASSIGNOP Exp
    // | error AND Exp
    // | error OR Exp
    // | error RELOP Exp
    // | error PLUS Exp
    // | error MINUS Exp
    // | error STAR Exp
    // | error DIV Exp
    ;
Args : Exp COMMA Args {$$ = buildAST("Args", 3, $1,$2,$3);}
    | Exp {$$ = buildAST("Args", 1, $1);}
    | error COMMA Args
    ;

%%
int yyerror(char* msg){
    AST_PRINT_FLAG = 0;
    //printf("error: %s\n",msg);
    printf("Error type B at Line %d: %s near %s\n", yylineno, msg,yytext);
}