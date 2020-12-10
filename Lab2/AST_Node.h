#ifndef AST_NODE_H
#define AST_NODE_H

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

Node* createNode(int line_number, char* name, char* value){
    Node* current_node = (Node*)malloc(sizeof(Node));
    current_node->children_index = 0;
    current_node->line_number = line_number;
    int name_length = strlen(name) + 1;
    current_node->name = (char*) malloc(sizeof(char) * name_length);
    strncpy(current_node->name, name, name_length);
    if(value != NULL){    // 说明是终结符号(token)
        int value_length = strlen(value) + 1;
        current_node->value = (char*) malloc(sizeof(char) * value_length);
        strncpy(current_node->value, value, value_length);
    }
    else
        current_node->value = NULL;
    return current_node;
}

void addChild(Node* father, Node* child){
    father->children[father->children_index] = child;
    father->children_index++;
}
#endif