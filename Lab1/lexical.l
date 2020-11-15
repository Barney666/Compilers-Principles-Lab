%{
    #include "syntax.tab.h"
    #include <stdio.h> 
    #include <string.h>
    int line_number = 1;
    int is_wrong = 0;
    char *ptr;   // 没用的，只是为了用一下strtol
    // 正则表达式中的ANNOTATE,INT,OCT,HEX,FLOAT,SNFLOAT,ID是重点
%}

STRUCT          "struct"
CLASS           "class"
IF              "if"
ELSE            "else"
SWITCH          "switch"
CASE            "case"
DEFAULT         "default"
FOR             "for"
WHILE           "while"
DO              "do"
CONTINUE        "continue"
BREAK           "break"
VOID            "void"
INT_TYPE        "int"
FLOAT_TYPE      "float"
CHAR            "char"
SIZEOF          "sizeof"
CONST           "const"
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
FLOAT	        {INT}\.{DIGIT}+
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
{STRUCT}          { fprintf(stdout, "STRUCT %s at Line %d.\n", yytext, line_number); }
{CLASS}           { fprintf(stdout, "CLASS %s at Line %d.\n", yytext, line_number); }
{IF}              { fprintf(stdout, "IF %s at Line %d.\n", yytext, line_number); }
{ELSE}            { fprintf(stdout, "ELSE %s at Line %d.\n", yytext, line_number); }
{SWITCH}          { fprintf(stdout, "SWITCH %s at Line %d.\n", yytext, line_number); }
{CASE}            { fprintf(stdout, "CASE %s at Line %d.\n", yytext, line_number); }
{DEFAULT}         { fprintf(stdout, "DEFAULT %s at Line %d.\n", yytext, line_number); }
{FOR}             { fprintf(stdout, "FOR %s at Line %d.\n", yytext, line_number); }
{WHILE}           { fprintf(stdout, "WHILE %s at Line %d.\n", yytext, line_number); }
{DO}              { fprintf(stdout, "DO %s at Line %d.\n", yytext, line_number); }
{CONTINUE}        { fprintf(stdout, "CONTINUE %s at Line %d.\n", yytext, line_number); }
{BREAK}           { fprintf(stdout, "BREAK %s at Line %d.\n", yytext, line_number); }
{VOID}            { fprintf(stdout, "VOID %s at Line %d.\n", yytext, line_number); }
{INT_TYPE}        { fprintf(stdout, "TYPE %s at Line %d.\n", yytext, line_number); }
{FLOAT_TYPE}      { fprintf(stdout, "TYPE %s at Line %d.\n", yytext, line_number); }
{CHAR}            { fprintf(stdout, "CHAR %s at Line %d.\n", yytext, line_number); }
{SIZEOF}          { fprintf(stdout, "SIZEOF %s at Line %d.\n", yytext, line_number); }
{CONST}           { fprintf(stdout, "CONST %s at Line %d.\n", yytext, line_number); }
{RETURN}          { fprintf(stdout, "RETURN %s at Line %d.\n", yytext, line_number); }
{NEXTLINE}        { line_number++; }
{WS}              {}
{ANNOTATE_SINGLE} { line_number++; }
{ANNOTATE_MULTI}  { 
                    int i = 0;
                    while(yytext[i]!='\0'){
                        if(yytext[i]=='\n'){
                            line_number++;
                        }
                        i++;
                    }
                  }
{INT}             { fprintf(stdout, "INT %s at Line %d.\n", yytext, line_number); }
{OCT}             { fprintf(stdout, "INT %d at Line %d.\n", strtol(yytext, &ptr, 8), line_number); }
{HEX}             { fprintf(stdout, "INT %d at Line %d.\n", strtol(yytext, &ptr, 16), line_number); }
{FLOAT}	          { fprintf(stdout, "FLOAT %f at Line %d.\n", atof(yytext), line_number); }
{SNFLOAT}         { fprintf(stdout, "FLOAT %f at Line %d.\n", atof(yytext), line_number); }
{ID}              { fprintf(stdout, "ID %s at Line %d.\n", yytext, line_number); }
{SEMI}            { fprintf(stdout, "SEMI %s at Line %d.\n", yytext, line_number); }
{COMMA}           { fprintf(stdout, "COMMA %s at Line %d.\n", yytext, line_number); }
{ASSIGNOP}        { fprintf(stdout, "ASSIGNOP %s at Line %d.\n", yytext, line_number); }
{RELOP}           { fprintf(stdout, "RELOP %s at Line %d.\n", yytext, line_number); }
{PLUS}            { fprintf(stdout, "PLUS %s at Line %d.\n", yytext, line_number); }
{MINUS}           { fprintf(stdout, "MINUS %s at Line %d.\n", yytext, line_number); }
{STAR}            { fprintf(stdout, "STAR %s at Line %d.\n", yytext, line_number); }
{DIV}             { fprintf(stdout, "DIV %s at Line %d.\n", yytext, line_number); }
{AND}             { fprintf(stdout, "AND %s at Line %d.\n", yytext, line_number); }
{OR}              { fprintf(stdout, "OR %s at Line %d.\n", yytext, line_number); }
{DOT}             { fprintf(stdout, "DOT %s at Line %d.\n", yytext, line_number); }
{NOT}             { fprintf(stdout, "NOT %s at Line %d.\n", yytext, line_number); }
{LP}              { fprintf(stdout, "LP %s at Line %d.\n", yytext, line_number); }
{RP}              { fprintf(stdout, "RP %s at Line %d.\n", yytext, line_number); }
{LB}              { fprintf(stdout, "LB %s at Line %d.\n", yytext, line_number); }
{RB}              { fprintf(stdout, "RB %s at Line %d.\n", yytext, line_number); }
{LC}              { fprintf(stdout, "LC %s at Line %d.\n", yytext, line_number); }
{RC}              { fprintf(stdout, "RC %s at Line %d.\n", yytext, line_number); }
.                 { 
                    is_wrong = 1;
                    fprintf(stderr, "Error type A at Line %d: Mysterious character \"%s\".\n", line_number, yytext); 
                  }

%%
int lex(const char* input) {
    freopen("src/out.txt", "w+", stdout);     // stdout的输出重定向，为了保存没有错误的地方，如果最终没有词法错误，会输出全部。
    yy_scan_string(input);
    yylex();
    return is_wrong;
}
int yywrap(){
    return 1;
}