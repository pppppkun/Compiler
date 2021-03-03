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
INT [-]?{nozero}+{digit}*|{ZERO}
FLOAT [-]?0+{nozero}*\.{digit}+
letter_ [A-Za-z_]
ID {letter_}({letter_}|{digit})*
WS [\f\r\t\v ]*
%%

{IF} {printf("IF\n");}
{ELSE} {printf("ELSE\n");}
{WHILE} {printf("WHILE\n");}
{STRUCT} {printf("STRUCT\n");}
{RETURN} {printf("RETURN\n");}
{TYPE} {printf("TYPE\n");}
{ID} {printf("ID\n");}
{SEMI} {printf("SEMI\n");}
{COMMA} {printf("COMMA\n");}
{ASSIGNOP} {printf("ASSIGNOP\n");}
{RELOP} {printf("RELOP\n");}
{PLUS} {printf("PLUS\n");}
{MINUS} {printf("MINUS\n");}
{STAR} {printf("STAR\n");}
{DIV} {printf("DIV\n");}
{AND} {printf("AND\n");}
{OR} {printf("OR\n");}
{DOT} {printf("DOT\n");}
{NOT} {printf("NOT\n");}
{LP} {printf("LP\n");}
{RP} {printf("RP\n");}
{LB} {printf("LB\n");}
{RB} {printf("RB\n");}
{LC} {printf("LC\n");}
{RC} {printf("RC\n");}
{INT} {printf("INT\n");}
{FLOAT} {printf("FLOAT\n");}
{WS} {}
\n {}
{ZERO}{digit}+ {printf("Error type A at Line %d: Mysterious character \"%s\"\n", yylineno, yytext);}
. {printf("Error type A at Line %d: Mysterious character \"%s\"\n", yylineno, yytext);}


