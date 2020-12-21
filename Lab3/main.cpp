#include <iostream>
#include <string>
#include <fstream>
#include "AST_Node.h"
//#include "semantic.h"

using namespace std;

extern "C" Node* root;
extern "C" struct yy_buffer_state * yy_scan_string ( const char *yy_str  );
extern "C" int yyparse();
extern void start_semantic(Node* root);

int lex_error = 0;
int syntax_error = 0;

void printTree(Node* current_node, int height) {
    if(current_node == NULL)
        return;
    for (int i = 0; i < height; i++) 
        fprintf(stderr,"  ");
    fprintf(stderr,"%s", current_node->name);
    if(current_node->children_index != 0){
        fprintf(stderr," (%d)\n", current_node->line_number);
        for (int i = 0; i < current_node->children_index; i++){
            printTree(current_node->children[i], height + 1);     // DFS           
        }
    }
    else{
        if(strcmp(current_node->name, "TYPE") == 0 || strcmp(current_node->name, "ID") == 0 || strcmp(current_node->name, "INT") == 0 || strcmp(current_node->name, "FLOAT") == 0)
            fprintf(stderr,": %s", current_node->value);
        fprintf(stderr, "\n");
    }
}


// .l中的全是终结符，.y中的全是非终结符
int main(int argc, char* argv[]) {
    ifstream file(argv[1]);
    string temp;
    string file_content = "";
    if (!file.is_open()) {
        cout << "未成功打开文件" << endl;
    }
    while (getline(file, temp)) {
        file_content.append(temp);
        file_content = file_content + '\n';
    }
    file.close();
    yy_scan_string(file_content.data());     // yyrestart要用FILE*，我不想用，就用了这个
    yyparse();
    if (!lex_error && !syntax_error) {
//        printTree(root, 0);
        start_semantic(root);
    }
    return 0;
}
