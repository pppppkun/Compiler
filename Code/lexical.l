%{
    #include "syntax.tab.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
      yylloc.first_line = yylloc.last_line = yylineno; \
      yylloc.first_column = yycolumn ; \
      yylloc.last_column = yycolumn + yyleng - 1;\
      yycolumn += yyleng;
    void my_debug(char *str, int line);
%}

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
COMMENT_LINE \/\/
COMMENT_LINES \/\*
COMMENT {COMMENT_LINES}|{COMMENT_LINE}
%%
{COMMENT} {
    my_debug("COMMENT", yylineno);
    int flag = 0;
    if(yytext[0]=='/' && yytext[1]=='*'){
        char c = input();
        while(1){
            if(c=='\0') {
                return DIV;
            }
            if(c=='\n') yylineno++;
            if(c == '*'){
                c = input();
                if(c == '/') break;
            }
            else c = input();
        }
    }
    else{
        char c = input();
        while(c!='\n') c = input();
        yylineno++;
    }
}
{IF} { my_debug("IF",yylineno); return IF;  }
{ELSE} { my_debug("ELSE",yylineno); return ELSE;  }
{WHILE} { my_debug("WHILE",yylineno); return WHILE;  }
{STRUCT} { my_debug("STRUCT",yylineno); return STRUCT;  }
{RETURN} { my_debug("RETURN",yylineno); return RETURN;  }
{TYPE} { my_debug("TYPE",yylineno); return TYPE;  }
{SEMI} { my_debug("SEMI",yylineno); return SEMI;  }
{COMMA} { my_debug("COMMA",yylineno); return COMMA;  }
{ASSIGNOP} { my_debug("ASSIGNOP",yylineno); return ASSIGNOP;  }
{RELOP} { my_debug("RELOP",yylineno); return RELOP;  }
{PLUS} { my_debug("PLUS",yylineno); return PLUS;   }
{MINUS} { my_debug("MINUS",yylineno); return MINUS;  }
{STAR} { my_debug("STAR",yylineno); return STAR; }
{DIV} { my_debug("DIV",yylineno); return DIV;  }
{AND} { my_debug("AND",yylineno); return AND;  }
{OR} { my_debug("OR",yylineno); return OR;  }
{DOT} { my_debug("DOT",yylineno); return DOT;  }
{NOT} { my_debug("NOT",yylineno); return NOT;  }
{LP} { my_debug("LP",yylineno); return LP;  }
{RP} { my_debug("RP",yylineno); return RP;  }
{LB} { my_debug("LB",yylineno); return LB;  }
{RB} { my_debug("RB",yylineno); return RB;  }
{LC} { my_debug("LC",yylineno); return LC;  }
{RC} { my_debug("RC",yylineno); return RC;  }
{INT} { my_debug("INT",yylineno); yylval.type_int = atoi(yytext); return INT;  }
{FLOAT} { my_debug("FLOAT",yylineno); yylval.type_float = atof(yytext); return FLOAT; }
{ID} { my_debug("ID",yylineno); yylval.type_string = yytext; return ID;  }
{WS} { }
\n { yycolumn = 1; yylineno++;}
. {printf("Error type A at Line %d: Mysterious character \"%s\"\n", yylineno, yytext);}

