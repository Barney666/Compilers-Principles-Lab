#ifndef SEMANTIC_H
#define SEMENTIC_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "AST_Node.h"

using namespace std;

// ******************************************常量与结构体******************************************

typedef enum kind { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;
typedef enum basic_type { BASIC_INT_TYPE, BASIC_FLOAT_TYPE } Basic_Type;

typedef struct type Type;
typedef struct symbol_table_item Symbol_Table_Item;

struct type {
    Kind kind;
    union {
        Basic_Type basic;        // 基本类型
        struct {                // 数组类型，信息包括元素类型、数组大小
            Type* element_type;
            int size;
        } array;
        struct {                // 结构体类型，所有信息存在一个vector中，但symbol_table中还是有这些item
            vector<Symbol_Table_Item>* all_item_vector;
            // 这里不是指针的话就会自动分配一堆内存以后vector.size()巨大，vector里面还啥也没有，所以就得用个指针，然后 = new vector<>()
            // 好像正常来说union里面不能用STL...
        } structure;
        struct {                // 函数类型，信息包括参数个数、参数类型、返回类型
            vector<Symbol_Table_Item>* all_arg_vector;   // 所有参数，个数可由.size()获得
            Type* return_type;
        } function;
    } u;
};

struct symbol_table_item{
    Type* type;
    string name;     // 这里的name是被定义的名称【主要是用于structure中】
    int row;         // 在哪行被定义的
};


// ********************************************全局变量********************************************

unordered_map<string, Symbol_Table_Item> symbol_table;      // key为被定义的名称，与value中的name值一样，这里只是为了寻找方便
Type* int_type_needed;
Type* float_type_needed;

// **********************************************函数**********************************************

// 辅助用的
void semantic_error(int error_type, int line_number, string name);
Node* get_vardec_id_node(Node* vardec);
Type* get_vardec_type(Node* vardec, Type* basic_type);
bool type_equal(Type* type1, Type* type2);

// 分析用的
void start_semantic(Node* root);
void ExtDefList(Node* node);
void ExtDef(Node* node);
Type* Specifier(Node* node);
Type* StructSpecifier(Node* node);
Type* VarDec(Node* node, Type* basic_type);
bool FunDec(Node* node, Type* return_type);
vector<Symbol_Table_Item>* VarList(Node* node);
Symbol_Table_Item ParamDec(Node* node);        // 这里其实有一点大材小用
void CompSt(Node *node, Type* return_type);
void StmtList(Node* node, Type* return_type);
void Stmt(Node* node, Type* return_type);
void DefList(Node *node);
void Def(Node* node);
// 无DecList与Dec
Type* Exp(Node* node);
vector<Type*> Args(Node* args_node);

#endif