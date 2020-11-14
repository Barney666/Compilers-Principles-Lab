%{
    #include "syntax.tab.h"
    #include <string>
%}

if              "if"
else            "else"
ws              [ \t\n\r]+
digit           [0-9]
letter          [A-Za-z]
integer         {digit}+
include         ({letter})({letter}|{digit})*

%%

{ws}            {}
{if}            { printf("IF");}


%%
void check(string )