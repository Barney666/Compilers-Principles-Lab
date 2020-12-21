%{
    #include "lex.yy.c"
    extern int syntax_error;
    extern int yylineno;
    struct AST_Node* root;
%}

// 终结符和非终结符的属性
%union{  
    struct AST_Node* node; 
}

// tokens
%token <node> INT OCT HEX
%token <node> FLOAT SNFLOAT
%token <node> ID
%token <node> SEMI COMMA
%token <node> ASSIGNOP
%token <node> RELOP
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR DOT NOT
%token <node> INT_TYPE FLOAT_TYPE
%token <node> LP RP LB RB LC RC
%token <node> STRUCT
%token <node> RETURN
%token <node> IF ELSE WHILE

// non-terminals

%type <node> Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type <node> Specifier StructSpecifier OptTag Tag   //  Specifiers
%type <node> VarDec FunDec VarList ParamDec         //  Declarators
%type <node> CompSt StmtList Stmt                   //  Statements
%type <node> DefList Def Dec DecList                //  Local Definitions
%type <node> Exp Args                               //  Expressions

// precedence and associativity

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS
%left LP RP LB RB DOT

%%
// High-level Definitions
Program:            ExtDefList                              { $$ = createNode(@$.first_line, "Program", NULL); addChild($$,$1); root = $$; }
    ;
ExtDefList:         ExtDef ExtDefList                       { $$ = createNode(@$.first_line, "ExtDefList", NULL); addChild($$,$1); addChild($$,$2); }
    |                                                       { $$ = NULL; }
    ;
ExtDef:             Specifier ExtDecList SEMI               { $$ = createNode(@$.first_line, "ExtDef", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Specifier SEMI                          { $$ = createNode(@$.first_line, "ExtDef", NULL); addChild($$,$1); addChild($$,$2); }
    |               Specifier FunDec CompSt                 { $$ = createNode(@$.first_line, "ExtDef", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               error SEMI                              { syntax_error = 1; }
    ;
ExtDecList:         VarDec                                  { $$ = createNode(@$.first_line, "ExtDecList", NULL); addChild($$,$1); }
    |               VarDec COMMA ExtDecList                 { $$ = createNode(@$.first_line, "ExtDecList", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    ;

// Specifiers
Specifier:          INT_TYPE                                { $$ = createNode(@$.first_line, "Specifier", NULL); addChild($$,$1); }
    |               FLOAT_TYPE                              { $$ = createNode(@$.first_line, "Specifier", NULL); addChild($$,$1); }
    |               StructSpecifier                         { $$ = createNode(@$.first_line, "Specifier", NULL); addChild($$,$1); }
    ;
StructSpecifier:    STRUCT OptTag LC DefList RC             { $$ = createNode(@$.first_line, "StructSpecifier", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); addChild($$,$5); }
    |               STRUCT Tag                              { $$ = createNode(@$.first_line, "StructSpecifier", NULL); addChild($$,$1); addChild($$,$2); }
    |               STRUCT OptTag LC error RC               { syntax_error = 1; }
    ;
OptTag:             ID                                      { $$ = createNode(@$.first_line, "OptTag", NULL); addChild($$,$1); }
    |                                                       { $$ = NULL; }
    ;
Tag:                ID                                      { $$ = createNode(@$.first_line, "Tag", NULL); addChild($$,$1); }
    ;

// Declarators
VarDec:             ID                                      { $$ = createNode(@$.first_line, "VarDec", NULL); addChild($$,$1); }
    |               VarDec LB INT RB                        { $$ = createNode(@$.first_line, "VarDec", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); }
    |               VarDec LB error RB                      { syntax_error = 1; }
    ;
FunDec:             ID LP VarList RP                        { $$ = createNode(@$.first_line, "FunDec", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); }
    |               ID LP RP                                { $$ = createNode(@$.first_line, "FunDec", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               ID LP error RP                          { syntax_error = 1; }
    ;
VarList:            ParamDec COMMA VarList                  { $$ = createNode(@$.first_line, "VarList", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               ParamDec                                { $$ = createNode(@$.first_line, "VarList", NULL); addChild($$,$1); }
    ;
ParamDec:           Specifier VarDec                        { $$ = createNode(@$.first_line, "ParamDec", NULL); addChild($$,$1); addChild($$,$2); }
    ;
// Statements
CompSt:             LC DefList StmtList RC                  { $$ = createNode(@$.first_line, "CompSt", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); }
    |               LC error RC                             { syntax_error = 1; }
    ;
StmtList:           Stmt StmtList                           { $$ = createNode(@$.first_line, "StmtList", NULL); addChild($$,$1); addChild($$,$2); }
    |                                                       { $$ = NULL; }
    ;
Stmt:               Exp SEMI                                { $$ = createNode(@$.first_line, "Stmt", NULL); addChild($$,$1); addChild($$,$2); }
    |               CompSt                                  { $$ = createNode(@$.first_line, "Stmt", NULL); addChild($$,$1); }
    |               RETURN Exp SEMI                         { $$ = createNode(@$.first_line, "Stmt", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = createNode(@$.first_line, "Stmt", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); addChild($$,$5); }
    |               IF LP Exp RP Stmt ELSE Stmt             { $$ = createNode(@$.first_line, "Stmt", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); addChild($$,$5); addChild($$,$6); addChild($$,$7); }
    |               WHILE LP Exp RP Stmt                    { $$ = createNode(@$.first_line, "Stmt", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); addChild($$,$5); }
    |               error SEMI                              { syntax_error = 1; }
    ;
// Local Definitions
DefList:            Def DefList                             { $$ = createNode(@$.first_line, "DefList", NULL); addChild($$,$1); addChild($$,$2); }
    |                                                       { $$ = NULL; }
    ;
Def:                Specifier DecList SEMI                  { $$ = createNode(@$.first_line, "Def", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               error SEMI                              { syntax_error = 1; }
    // one of hard-six
    ;
DecList:            Dec                                     { $$ = createNode(@$.first_line, "DecList", NULL); addChild($$,$1); }
    |               Dec COMMA DecList                       { $$ = createNode(@$.first_line, "DecList", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    ;
Dec:                VarDec                                  { $$ = createNode(@$.first_line, "Dec", NULL); addChild($$,$1); }
    |               VarDec ASSIGNOP Exp                     { $$ = createNode(@$.first_line, "Dec", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    ;
//7.1.7 Expressions
Exp:                Exp ASSIGNOP Exp                        { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp AND Exp                             { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp OR Exp                              { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp RELOP Exp                           { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp PLUS Exp                            { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp MINUS Exp                           { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp STAR Exp                            { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp DIV Exp                             { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               LP Exp RP                               { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               MINUS Exp %prec UMINUS                  { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); }
    |               NOT Exp                                 { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); }
    |               ID LP Args RP                           { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); }
    |               ID LP RP                                { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp LB Exp RB                           { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); addChild($$,$4); }
    |               Exp DOT ID                              { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               ID                                      { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); }
    |               INT                                     { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); }
    |               OCT                                     { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); }
    |               HEX                                     { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); }
    |               FLOAT                                   { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); }
    |               SNFLOAT                                 { $$ = createNode(@$.first_line, "Exp", NULL); addChild($$,$1); }
    |               LP error RP                             { syntax_error = 1; }
    |               ID LP error RP                          { syntax_error = 1; }
    |               Exp LB error RB                         { syntax_error = 1; }
    ;
Args :              Exp COMMA Args                          { $$ = createNode(@$.first_line, "Args", NULL); addChild($$,$1); addChild($$,$2); addChild($$,$3); }
    |               Exp                                     { $$ = createNode(@$.first_line, "Args", NULL); addChild($$,$1); }
    ; 
%%
void yyerror(char* msg) {
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}