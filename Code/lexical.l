%option yylineno

IF if
ELSE else
WHILE while
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|<=|>=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
digit [0-9]
nozero [1-9]
ZERO 0
NO_SIGN {nozero}{digit}*|{ZERO}
INT [+-]?{NO_SIGN}
FLOAT {INT}\.{digit}+
ILL_INT {ZERO}+{digit}+
letter_ [A-Za-z_]
CHAR_IN_NUM [a-fA-F]
ID {letter_}({letter_}|{digit})*
WS [\f\r\t\v ]*
%%

{IF} {    }
{ELSE} {    }
{WHILE} {    }
{STRUCT} {    }
{RETURN} {    }
{TYPE} {    }
{ID} {    }
{SEMI} {    }
{COMMA} {    }
{ASSIGNOP} {    }
{RELOP} {    }
{PLUS} {    }
{MINUS} {    }
{STAR} {    }
{DIV} {    }
{AND} {    }
{OR} {    }
{DOT} {    }
{NOT} {    }
{LP} {    }
{RP} {    }
{LB} {    }
{RB} {    }
{LC} {    }
{RC} {    }
{INT} {    }
{FLOAT} {  }
{WS} {}
\n {}
{ILL_INT} {printf("Error type A at Line %d: illegal int \"%s\"\n", yylineno, yytext);}
({digit}*{DOT})|({DOT}{digit}*) {printf("Error type A at Line %d: illegal float \"%s\"\n", yylineno, yytext);}
. {printf("Error type A at Line %d: Mysterious character \"%s\"\n", yylineno, yytext);}


