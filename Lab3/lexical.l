%{
    #include <stdio.h> 
    #include <string.h>
    #include "syntax.tab.h"
    #include "AST_Node.h"
    extern int lex_error;
    char *ptr;   // 没用的，只是为了后面用一下strtol
    int yycolumn = 1;    // 维护Bison中的位置信息，下同
    #define YY_USER_ACTION \
                yylloc.first_line = yylloc.last_line = yylineno; \ 
                yylloc.first_column = yycolumn; \ 
                yylloc.last_column = yycolumn + yyleng - 1; \ 
                yycolumn += yyleng;
%}

%option yylineno
%option yywrap

STRUCT          "struct"
IF              "if"
ELSE            "else"
WHILE           "while"
INT_TYPE        "int"
FLOAT_TYPE      "float"
RETURN          "return"
NEXTLINE        "\n"
WS              [ \t\r]+
ANNOTATE_SINGLE "//".*"\n"
ANNOTATE_MULTI  "/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"
DIGIT           [0-9]
LETTER          [_A-Za-z]
INT             0|([1-9]{DIGIT}*)
OCT             0([0-7]+)
HEX             0(x|X)[0-9a-fA-F]+
FLOAT	          {INT}\.{DIGIT}+
SNFLOAT         {DIGIT}*\.{DIGIT}*[Ee][+-]?{DIGIT}+
ID              ({LETTER})({LETTER}|{DIGIT})*
SEMI            ";"
COMMA           ","
ASSIGNOP        "="
RELOP           ">"|"<"|">="|"<="|"=="|"!="
PLUS            "+"
MINUS           "-"
STAR            "*"
DIV             "/"
AND             "&&"
OR              "||"
DOT             "."
NOT             "!"
LP              "("
RP              ")"
LB              "["
RB              "]"
LC              "{"
RC              "}"

%%
{STRUCT}          { yylval.node = createNode(yylineno, "STRUCT", yytext); return STRUCT; }
{IF}              { yylval.node = createNode(yylineno, "IF", yytext); return IF;  }
{ELSE}            { yylval.node = createNode(yylineno, "ELSE", yytext); return ELSE; }
{WHILE}           { yylval.node = createNode(yylineno, "WHILE", yytext); return WHILE; }
{INT_TYPE}        { yylval.node = createNode(yylineno, "TYPE", yytext); return INT_TYPE; }
{FLOAT_TYPE}      { yylval.node = createNode(yylineno, "TYPE", yytext); return FLOAT_TYPE; }
{RETURN}          { yylval.node = createNode(yylineno, "RETURN", yytext); return RETURN; }
{NEXTLINE}        { yycolumn = 1; }
{WS}              {}
{ANNOTATE_SINGLE} { yycolumn = 1; }
{ANNOTATE_MULTI}  {
                    int i = 0;
                    while(yytext[i]!='\0'){
                        if(yytext[i]=='\n'){
                            yycolumn = 1;
                        }
                        i++;
                    }
                  }
{INT}             { yylval.node = createNode(yylineno, "INT", yytext); return INT; }
{OCT}             {
                    char buffer[64];
                    sprintf(buffer, "%d", strtol(yytext, &ptr, 8));
                    yylval.node = createNode(yylineno, "INT", buffer);
                    return INT;
                  }
{HEX}             {
                    char buffer[64];
                    sprintf(buffer, "%d", strtol(yytext, &ptr, 16));
                    yylval.node = createNode(yylineno, "INT", buffer);
                    return INT;
                  }
{FLOAT}	          {
                    char buffer[64];
                    sprintf(buffer, "%f", atof(yytext));
                    yylval.node = createNode(yylineno, "FLOAT", buffer);
                    return FLOAT;
                  }
{SNFLOAT}         {
                    char buffer[64];
                    sprintf(buffer, "%f", atof(yytext));
                    yylval.node = createNode(yylineno, "FLOAT", buffer);
                    return FLOAT;
                  }
{ID}              { yylval.node = createNode(yylineno, "ID", yytext); return ID; }
{SEMI}            { yylval.node = createNode(yylineno, "SEMI", yytext); return SEMI; }
{COMMA}           { yylval.node = createNode(yylineno, "COMMA", yytext); return COMMA; }
{ASSIGNOP}        { yylval.node = createNode(yylineno, "ASSIGNOP", yytext); return ASSIGNOP; }
{RELOP}           { yylval.node = createNode(yylineno, "RELOP", yytext); return RELOP; }
{PLUS}            { yylval.node = createNode(yylineno, "PLUS", yytext); return PLUS; }
{MINUS}           { yylval.node = createNode(yylineno, "MINUS", yytext); return MINUS; }
{STAR}            { yylval.node = createNode(yylineno, "STAR", yytext); return STAR; }
{DIV}             { yylval.node = createNode(yylineno, "DIV", yytext); return DIV; }
{AND}             { yylval.node = createNode(yylineno, "AND", yytext); return AND; }
{OR}              { yylval.node = createNode(yylineno, "OR", yytext); return OR; }
{DOT}             { yylval.node = createNode(yylineno, "DOT", yytext); return DOT; }
{NOT}             { yylval.node = createNode(yylineno, "NOT", yytext); return NOT; }
{LP}              { yylval.node = createNode(yylineno, "LP", yytext); return LP; }
{RP}              { yylval.node = createNode(yylineno, "RP", yytext); return RP; }
{LB}              { yylval.node = createNode(yylineno, "LB", yytext); return LB; }
{RB}              { yylval.node = createNode(yylineno, "RB", yytext); return RB; }
{LC}              { yylval.node = createNode(yylineno, "LC", yytext); return LC; }
{RC}              { yylval.node = createNode(yylineno, "RC", yytext); return RC; }
.                 { 
                    lex_error = 1;
                    fprintf(stderr, "Error type A at Line %d: Mysterious character \"%s\".\n", yylineno, yytext); 
                  }

%%