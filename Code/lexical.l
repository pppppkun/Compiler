%{
    #include "syntax.tab.h"
    #include "main.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
      yylloc.first_line = yylloc.last_line = yylineno; \
      yylloc.first_column = yycolumn ; \
      yylloc.last_column = yycolumn + yyleng - 1;\
      yycolumn += yyleng;
    void my_debug(char *str, int line);
    int buildAST(char* name, int childSum, ...);
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
{IF} { yylval=buildAST("IF", 0, yylineno);my_debug("IF",yylineno); return IF;  }
{ELSE} { yylval=buildAST("ELSE", 0, yylineno);my_debug("ELSE",yylineno); return ELSE;  }
{WHILE} { yylval=buildAST("WHILE",0,yylineno);my_debug("WHILE",yylineno); return WHILE;  }
{STRUCT} { yylval=buildAST("STRUCT",0,yylineno);my_debug("STRUCT",yylineno); return STRUCT;  }
{RETURN} { yylval=buildAST("RETURN",0,yylineno);my_debug("RETURN",yylineno); return RETURN;  }
{TYPE} { yylval=buildAST("TYPE", 0, yylineno);my_debug("TYPE",yylineno); return TYPE;  }
{SEMI} { yylval=buildAST("SEMI",0,yylineno);my_debug("SEMI",yylineno); return SEMI;  }
{COMMA} { yylval=buildAST("COMMA",0,yylineno);my_debug("COMMA",yylineno); return COMMA;  }
{ASSIGNOP} { yylval=buildAST("ASSIGNOP", 0, yylineno);my_debug("ASSIGNOP",yylineno); return ASSIGNOP;  }
{RELOP} { yylval=buildAST("RELOP",0,yylineno);my_debug("RELOP",yylineno); return RELOP;  }
{PLUS} { yylval=buildAST("PLUS",0,yylineno);my_debug("PLUS",yylineno); return PLUS;   }
{MINUS} { yylval=buildAST("MINUS",0,yylineno);my_debug("MINUS",yylineno); return MINUS;  }
{STAR} { yylval=buildAST("STAR",0,yylineno);my_debug("STAR",yylineno); return STAR; }
{DIV} { yylval=buildAST("DIV",0,yylineno);my_debug("DIV",yylineno); return DIV;  }
{AND} { yylval=buildAST("AND",0,yylineno);my_debug("AND",yylineno); return AND;  }
{OR} { yylval=buildAST("OR",0,yylineno);my_debug("OR",yylineno); return OR;  }
{DOT} { yylval=buildAST("DOT",0,yylineno);my_debug("DOT",yylineno); return DOT;  }
{NOT} { yylval=buildAST("NOT",0,yylineno);my_debug("NOT",yylineno); return NOT;  }
{LP} { yylval=buildAST("LP",0,yylineno);my_debug("LP",yylineno); return LP;  }
{RP} { yylval=buildAST("RP",0,yylineno);my_debug("RP",yylineno); return RP;  }
{LB} { yylval=buildAST("LB",0,yylineno);my_debug("LB",yylineno); return LB;  }
{RB} { yylval=buildAST("RB",0,yylineno);my_debug("RB",yylineno); return RB;  }
{LC} { yylval=buildAST("LC",0,yylineno);my_debug("LC",yylineno); return LC;  }
{RC} { yylval=buildAST("RC",0,yylineno);my_debug("RC",yylineno); return RC;  }
{INT} { yylval=buildAST("INT",0,yylineno);my_debug("INT",yylineno);  return INT;  }
{FLOAT} { yylval=buildAST("FLOAT",0,yylineno);my_debug("FLOAT",yylineno); ; return FLOAT; }
{ID} { yylval=buildAST("ID",0,yylineno);my_debug("ID",yylineno);  return ID;  }
{WS} { }
\n { yycolumn = 1; yylineno++;}
. {printf("Error type A at Line %d: Mysterious character \"%s\"\n", yylineno, yytext);}

