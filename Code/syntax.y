%locations
%{
    #include "lex.yy.c"
    #include <stdio.h>
    int yyerror(char* msg);
%}

%token INT
%token ADD SUB MUL DIV

%%

Calc :
    | Exp {printf("= %d\n", $1);}
    ;
Exp : Factor
    | Exp ADD Factor {$$ = $1 + $3;}
    | Exp SUB Factor {$$ = $1 - $3;}
    ;
Factor : Term
    | Factor MUL Term {$$ = $1 * $3;}
    | Factor DIV Term {$$ = $1 / $3;}
Term : INT

%%
int yyerror(char* msg){
    //printf("error: %s\n",msg);
    printf("Error type B at Line %d: %s\n", yylineno, msg);
}