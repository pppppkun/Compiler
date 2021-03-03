%option yylineno

IF if
ELSE else
%%

{IF} {printf("if%d\n", yylineno);}

\n { }
