%{
    #include "syntax.tab.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
      yylloc.first_line = yylloc.last_line = yylineno; \
      yylloc.first_column = yycolumn ; \
      yylloc.last_column = yycolumn + yyleng - 1;\
      yycolumn += yyleng;
%}

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
INT {NO_SIGN}
FLOAT {INT}\.{digit}+
ILL_INT {ZERO}+{digit}+
letter_ [A-Za-z_]
CHAR_IN_NUM [a-fA-F]
ID {letter_}({letter_}|{digit})*
WS [\f\r\t\v ]*
COMMENT_LINE \/\/.*
COMMENT_LINES \/\*(.|\n)*\*\/
COMMENT {COMMENT_LINES}|{COMMENT_LINE}
%%

{IF} {  return IF;  }
{ELSE} {  return ELSE;  }
{WHILE} {  return WHILE;  }
{STRUCT} {  return STRUCT;  }
{RETURN} {  return RETURN;  }
{TYPE} {  return TYPE;  }
{SEMI} {  return SEMI;  }
{COMMA} {  return COMMA;  }
{ASSIGNOP} {  return ASSIGNOP;  }
{RELOP} {  return RELOP;  }
{PLUS} { return PLUS;   }
{MINUS} {  return MINUS;  }
{STAR} {  return STAR;  }
{DIV} {  return DIV;  }
{AND} {  return AND;  }
{OR} {  return OR;  }
{DOT} {  return DOT;  }
{NOT} {  return NOT;  }
{LP} {  return LP;  }
{RP} {  return RP;  }
{LB} {  return LB;  }
{RB} {  return RB;  }
{LC} {  return LC;  }
{RC} {  return RC;  }
{INT} {  yylval.type_int = atoi(yytext); return INT;  }
{FLOAT} { yylval.type_float = atof(yytext); return FLOAT; }
{ID} {  yylval.type_string = yytext; return ID;  }
{WS} { }
{COMMENT} {
    for(int i = 0;i<yyleng;i++){
        if(yytext[i]=='\n') yylineno++;
    }
}
\n { yycolumn = 1;}
. {printf("Error type A at Line %d: Mysterious character \"%s\"\n", yylineno, yytext);}

