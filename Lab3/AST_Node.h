#ifndef _AST_NODE_H_
#define _AST_NODE_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct AST_Node{
    int line_number;
    char* name;
    char* value;
    struct AST_Node* children[10];    // 所有子节点
    int children_index;
} Node;   // 使用typedef在后续构造变量的时候就可以省去struct关键字，即用Node代替struct AST_Node

Node* createNode(int line_number, char* name, char* value);

void addChild(Node* father, Node* child);

#endif