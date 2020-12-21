#include "semantic.h"

void semantic_error(int error_type, int line_number, string name){
    string error_message;
    if(error_type == 1)
        error_message = "Undefined variable \""+ name +"\".";
    else if(error_type == 2)
        error_message = "Undefined function \""+ name +"\".";
    else if(error_type == 3)
        error_message = "Redefined variable \""+ name +"\".";
    else if(error_type == 4)
        error_message = "Redefined function \""+ name +"\".";
    else if(error_type == 5)
        error_message = "Type mismatched for assignment.";
    else if(error_type == 6)
        error_message = "The left-hand side of an assignment must be a variable.";
    else if(error_type == 7)
        error_message = "Type mismatched for operands.";
    else if(error_type == 8)
        error_message = "Type mismatched for return.";
    else if(error_type == 9)
        error_message = "Function \""+ name +"\" is not applicable for arguments followed.";
    else if(error_type == 10)
        error_message = "\""+ name +"\" is not an array.";
    else if(error_type == 11)
        error_message = "\""+ name +"\" is not a function.";
    else if(error_type == 12)
        error_message = "\""+ name +"\" is not an integer.";
    else if(error_type == 13)
        error_message = "Illegal use of \".\".";
    else if(error_type == 14)
        error_message = "Non-existent field \""+ name +"\".";
    else if(error_type == 15){      //two different error report format？
        if(name == "")
            error_message = "Field cannot be initialized.";
        else
        error_message = "Redefined field \""+ name +"\".";
    }
    else if(error_type == 16)
        error_message = "Duplicated name \""+ name +"\".";
    else if(error_type == 17)
        error_message = "Undefined structure \""+ name +"\".";

    cerr << "Error type " << to_string(error_type) << " at Line " << to_string(line_number) << ": ";
    cerr << error_message << endl;
}

Node* get_vardec_id_node(Node* vardec){    // VarDec -> ID
    Node* temp = vardec;
    while (strcmp(temp->name, "ID") != 0)          // 不是ID，往下递归
        temp = temp->children[0];
    return temp;
}
Type* get_vardec_type(Node* vardec, Type* basic_type){
    Type* vardec_type = basic_type;        // VarDec -> ID
    Node* temp = vardec;
    while (temp->children_index == 4){     // VarDec -> VarDec LB INT RB
        Type* new_type = (Type*)malloc(sizeof(Type));
        new_type->kind = ARRAY;
        new_type->u.array.size = atoi(temp->children[2]->value);
        new_type->u.array.element_type = vardec_type;
        vardec_type = new_type;            // 可能是多维的数组
        temp = temp->children[0];          // 可能是多维的数组
    }
    return vardec_type;
}

bool type_equal(Type* type1, Type* type2){
    if(type1 == NULL || type2 == NULL)     // 这里是因为之前有错误，所有才有type是NULL的，已经被发现过了这个错误
        return true;
    if(type1->kind != type2->kind)
        return false;
    else{
        if(type1->kind == BASIC)
            return type1->u.basic == type2->u.basic;
        else if(type1->kind == ARRAY)
            return type_equal(type1->u.array.element_type, type2->u.array.element_type);
        else if(type1->kind == STRUCTURE){
            vector<Symbol_Table_Item> all_item_vector1 = *type1->u.structure.all_item_vector;
            vector<Symbol_Table_Item> all_item_vector2 = *type2->u.structure.all_item_vector;
            if(all_item_vector1.size() != all_item_vector2.size())
                return false;
            for(int i = 0; i < all_item_vector1.size(); i++)
                if(!type_equal(all_item_vector1[i].type, all_item_vector2[i].type))
                    return false;
            return true;
        }
    }
}


// ###############################################################################################################


void start_semantic(Node* root){
    int_type_needed = (Type*)malloc(sizeof(Type));      // 指针得先这么整一下
    float_type_needed = (Type*)malloc(sizeof(Type));
    int_type_needed->kind = BASIC;
    int_type_needed->u.basic = BASIC_INT_TYPE;
    float_type_needed->kind = BASIC;
    float_type_needed->u.basic = BASIC_FLOAT_TYPE;
    // Program -> ExtDefList
    ExtDefList(root->children[0]);
}

void ExtDefList(Node* node){
    Node* temp = node;
    while (true){
        if(temp == NULL)
            break;
        else{
            ExtDef(temp->children[0]);
            temp = temp->children[1];
        }
    }
}

void ExtDef(Node* node){
    Type* basic_type = Specifier(node->children[0]);
    // ExtDef -> Specifier ExtDecList SEMI
    //                     ExtDecList -> VarDec
    //                                 | VarDec COMMA ExtDecList
    if(strcmp(node->children[1]->name, "ExtDecList") == 0){
        Node* ExtDecList_node = node->children[1];
        while(true){
            VarDec(ExtDecList_node->children[0], basic_type);
            if(ExtDecList_node->children_index > 1)
                ExtDecList_node = ExtDecList_node->children[2];
            else break;
        }
    }
    // ExtDef -> Specifier FunDec CompSt
    else if(strcmp(node->children[1]->name, "FunDec") == 0){
        if(!FunDec(node->children[1], basic_type))    // 如果是重复定义就不进去结构体里面了
            CompSt(node->children[2], basic_type);
    }
    // ExtDef -> Specifier SEMI has done in Specifier()
}

Type* Specifier(Node* node){
    Node* type_node = node->children[0];
    if(strcmp(type_node->name, "TYPE") == 0){
        if(strcmp(type_node->value, "int") == 0)
            return int_type_needed;
        else if(strcmp(type_node->value, "float") == 0)
            return float_type_needed;
    }
    else if(strcmp(type_node->name, "StructSpecifier") == 0)
        return StructSpecifier(type_node);
}

Type* StructSpecifier(Node* node){
    Type* struct_type = (Type*)malloc(sizeof(Type));
    struct_type->kind = STRUCTURE;
    struct_type->u.structure.all_item_vector = new vector<Symbol_Table_Item>;
    // StructSpecifier -> STRUCT OptTag LC DefList RC
    //                                     DefList -> Def DefList
    //                                                Def -> Specifier DecList SEMI
    //                                                                 DecList -> Dec
    //                                                                         |  Dec COMMA DecList
    //                                                                            Dec -> VarDec
    //                                                                                |  VarDec ASSIGNOP Exp
    if(node->children_index == 5){
        Node* DefList = node->children[3];
        while(DefList != NULL && DefList->children_index == 2){
            Node* Def_node = DefList->children[0];
            Type* basic_type = Specifier(Def_node->children[0]);
            Node* Declist_node = Def_node->children[1];
            while(true){
                Node* Dec_node = Declist_node->children[0];
                Node* Vardec_node = Dec_node->children[0];
                if(Dec_node->children_index == 1){
                    Symbol_Table_Item item;
                    Node* vardec_id_node = get_vardec_id_node(Vardec_node);
                    item.name = vardec_id_node->value;
                    item.row = vardec_id_node->line_number;
                    item.type = get_vardec_type(Vardec_node, basic_type);
                    bool duplicated= false;
                    for(auto vector_item: *(struct_type->u.structure.all_item_vector)){
                        if(vector_item.name == item.name){
                            semantic_error(15, Dec_node->line_number, item.name);
                            duplicated = true;
                            break;
                        }
                    }
                    if(!duplicated){
                        struct_type->u.structure.all_item_vector->push_back(item);
                        symbol_table.insert(pair<string, Symbol_Table_Item>(item.name, item));
                    }
                }
                // Dec -> VarDec ASSIGNOP Exp  结构体内不让赋值
                else if(Dec_node->children_index == 3){
                    semantic_error(15, Dec_node->line_number, "");
                }
                if(Declist_node->children_index > 1){
                    Declist_node = Declist_node->children[2];
                }
                else break;
            }
            DefList = DefList->children[1];
        }
        if(node->children[1] != NULL){     // OptTag -> ID
            Node* id_opttag_node = node->children[1]->children[0];
            auto iterator = symbol_table.find(id_opttag_node->value);
            if(iterator != symbol_table.end())
                semantic_error(16, id_opttag_node->line_number, id_opttag_node->value);
            else{
                Symbol_Table_Item new_item;
                new_item.name = id_opttag_node->value;
                new_item.type = struct_type;
                symbol_table.insert(pair<string, Symbol_Table_Item>(new_item.name, new_item));
            }
        }
        // OptTag -> empty 匿名结构体在返回后看Def中有没有名字再insert这个struct
        // struct{}; || struct{}a;
    }
    // StructSpecifier -> STRUCT Tag
    //                           Tag -> ID
    else if(node->children_index == 2){
        char* name = node->children[1]->children[0]->value;
        auto iterator = symbol_table.find(name);
        if(iterator == symbol_table.end()){
            semantic_error(17, node->line_number, name);
            return NULL;
        }
        else
            struct_type = iterator->second.type;
    }
    return struct_type;
}

Type* VarDec(Node* node, Type* basic_type){
    // VarDec -> ID
    //        |  VarDec LB INT RB
    Symbol_Table_Item item;
    Node* vardec_id_node = get_vardec_id_node(node);
    item.name = vardec_id_node->value;
    item.row = vardec_id_node->line_number;
    item.type = get_vardec_type(node, basic_type);
    auto iterator = symbol_table.find(item.name);
    if(iterator != symbol_table.end())
        semantic_error(3, item.row, item.name);
    else
        symbol_table.insert(pair<string, Symbol_Table_Item>(item.name, item));
    return item.type;
}

bool FunDec(Node* node, Type* return_type){
    // FunDec -> ID LP VarList RP
    //        |  ID LP RP
    string name = node->children[0]->value;
    int line_number = node->children[0]->line_number;
    auto iterator = symbol_table.find(name);
    if(iterator != symbol_table.end()){
        semantic_error(4, line_number, name);
        return true;
    }
    else{
        auto* all_arg_vector = new vector<Symbol_Table_Item>();
        if(node->children_index == 4)
            all_arg_vector = VarList(node->children[2]);
        Symbol_Table_Item func_item;
        func_item.name = name;
        func_item.row = line_number;
        Type* type = (Type*)malloc(sizeof(Type));
        type->kind = FUNCTION;
        type->u.function.all_arg_vector = all_arg_vector;
        type->u.function.return_type = return_type;
        func_item.type = type;
        symbol_table.insert(pair<string, Symbol_Table_Item>(name, func_item));
        return false;
    }
}

vector<Symbol_Table_Item>* VarList(Node* node){
    // VarList -> ParamDec COMMA VarList
    //         |  ParamDec
    auto* all_arg_vector = new vector<Symbol_Table_Item>();
    while (true){
        all_arg_vector->push_back(ParamDec(node->children[0]));
        if(node->children_index == 1)
            break;
        else
            node = node->children[2];
    }
    return all_arg_vector;
}

Symbol_Table_Item ParamDec(Node* node){
    // ParamDec -> Specifier VarDec
    Type* basic_type = Specifier(node->children[0]);
    Type* arg_type = VarDec(node->children[1], basic_type);
    Symbol_Table_Item arg;
    arg.type = arg_type;
    arg.name = get_vardec_id_node(node->children[1])->value;
    return arg;
}

void CompSt(Node* node, Type* return_type){
    // CompSt -> LC DefList StmtList RC
    DefList(node->children[1]);
    StmtList(node->children[2], return_type);
}

void StmtList(Node* node, Type* return_type){
    // StmtList -> Stmt StmtList
    //          |
    Node* temp = node;
    while (true){
        if(temp == NULL)
            break;
        Stmt(temp->children[0], return_type);
        temp = temp->children[1];
    }
}

void Stmt(Node* node, Type* return_type){
    // Stmt -> Exp SEMI
    if (strcmp(node->children[0]->name, "Exp") == 0)
        Exp(node->children[0]);
    // Stmt -> CompSt
    else if (strcmp(node->children[0]->name, "CompSt") == 0)
        CompSt(node->children[0], return_type);
    // Stmt -> RETURN Exp SEMI
    else if (strcmp(node->children[0]->name, "RETURN") == 0){
        if(!type_equal(return_type, Exp(node->children[1])))        // 检查返回值类型是否正确
            semantic_error(8, node->line_number, "");
    }
    // Stmt -> IF LP Exp RP Stmt
    else if (strcmp(node->children[0]->name, "IF") == 0){
        if(!type_equal(int_type_needed, Exp(node->children[2])))    // 只允许int做逻辑运算
            semantic_error(7, node->line_number, "");
        Stmt(node->children[4], return_type);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (node->children_index == 7)
            Stmt(node->children[6], return_type);
    }
    // Stmt -> WHILE LP Exp RP Stmt
    else if (strcmp(node->children[0]->name, "WHILE") == 0){
        if(!type_equal(int_type_needed, Exp(node->children[2])))    // 只允许int做逻辑运算
            semantic_error(7, node->line_number, "");
        Stmt(node->children[4], return_type);
    }
}

void DefList(Node* node){
    // DefList -> Def DefList
    //         |
    Node* temp = node;
    while(temp != NULL){     // 反正一定是2个子节点，如果children[1]是空，直接就是NULL，但此时children_index还是2
        Def(temp->children[0]);
        temp = temp->children[1];
    }
}

void Def(Node* node){   // 这的Declist和Dec其实可以拆分出去的
    // Def -> Specifier DecList SEMI
    Type* specifier_type = Specifier(node->children[0]);
    Node* Declist_node = node->children[1];
    // Def -> Specifier -> StructSpecifier -> STRUCT NULL LC DefList RC
    if(specifier_type != NULL && specifier_type->kind == STRUCTURE
    && (node->children[0]->children[0]->children_index == 5)
    && (node->children[0]->children[0]->children[1] == NULL)){
        //DecList -> Dec -> VarDec -> ID
        if(Declist_node->children_index == 1 && Declist_node->children[0]->children_index == 1
        && Declist_node->children[0]->children[0]->children_index == 1
        && strcmp(Declist_node->children[0]->children[0]->children[0]->name, "ID")){
            // 这种就是struct{}a;这样的脑瘫形式
            string struct_name = Declist_node->children[0]->children[0]->children[0]->value;
            auto iterator = symbol_table.find(struct_name);
            if(iterator != symbol_table.end())
                semantic_error(16, Declist_node->children[0]->children[0]->children[0]->line_number, struct_name);
            else{
                Symbol_Table_Item new_item;
                new_item.name = struct_name;
                new_item.type = specifier_type;
                symbol_table.insert(pair<string, Symbol_Table_Item>(new_item.name, new_item));
            }
        }
        else{  // 真·匿名 struct{};
            time_t current_time = time(NULL);
            Symbol_Table_Item new_item;
            new_item.name = to_string(current_time);        // 用当前时间戳来当匿名函数的名字，必不会重复
            new_item.type = specifier_type;
            symbol_table.insert(pair<string, Symbol_Table_Item>(new_item.name, new_item));
        }
    }
    while(true){
        // DecList -> Dec
        //         |  Dec COMMA DecList
        Node* Dec_node = Declist_node->children[0];
        // Dec -> VarDec
        //     |  VarDec ASSIGNOP Exp
        Node* Vardec_node = Dec_node->children[0];
        if(Dec_node->children_index == 1)
            VarDec(Vardec_node, specifier_type);
        else if(Dec_node->children_index == 3){
            Type* type1 = VarDec(Vardec_node, specifier_type);
            Type* type2 = Exp(Dec_node->children[2]);
            if(!type_equal(type1, type2))     // 检查赋值类型匹配
                semantic_error(5, Dec_node->line_number, "");
        }
        if(Declist_node->children_index == 3)
            Declist_node = Declist_node->children[2];
        else break;
    }
}

Type* Exp(Node* node){
    // Exp -> ID | INT | FLOAT
    if(node->children_index == 1){
        if(strcmp(node->children[0]->name, "ID") == 0){
            auto iterator = symbol_table.find(node->children[0]->value);
            if(iterator == symbol_table.end()){
                semantic_error(1, node->line_number, node->children[0]->value);
                return NULL;
            }
            else{
                return iterator->second.type;
            }
        }
        else if(strcmp(node->children[0]->name, "INT") == 0){
            return int_type_needed;
        }
        else if(strcmp(node->children[0]->name, "FLOAT") == 0){
            return float_type_needed;
        }
    }
    else if(node->children_index == 3){
        if(strcmp(node->children[0]->name, node->children[2]->name) == 0){
            // Exp -> Exp xx Exp
            string operation = node->children[1]->name;
            Type* type1 = Exp(node->children[0]);
            Type* type2 = Exp(node->children[2]);
            if(operation == "PLUS" || operation == "MINUS" || operation == "STAR" || operation == "DIV"){
                if(type_equal(type1, type2)){
                    if(type1 == NULL || type2 == NULL)
                        return NULL;
                    else return type1;
                }
                else{
                    semantic_error(7, node->line_number, "");
                    return NULL;
                }
            }
            else if(operation == "AND" || operation == "OR"){
                if(type1 == NULL || type2 == NULL)
                    return NULL;
                if(type1->u.basic != BASIC_INT_TYPE || type2->u.basic != BASIC_INT_TYPE){
                    semantic_error(7, node->line_number, "");
                    return NULL;
                }
                else{
                    return int_type_needed;
                }
            }
            else if(operation == "RELOP"){
                if(type_equal(type1, type2)){
                    return int_type_needed;
                }
                else{
                    semantic_error(7, node->line_number, "");
                    return NULL;
                }
            }
            else if(operation == "ASSIGNOP"){
                Node* exp = node->children[0];
                if((exp->children_index == 1 && strcmp(exp->children[0]->name, "ID") == 0)
                   ||( exp->children_index == 3 && strcmp(exp->children[1]->name, "DOT") == 0)
                   || (exp->children_index == 4 && strcmp(exp->children[1]->name, "LB") == 0)){
                    if(type_equal(type1, type2))
                        return type1;
                    else{
                        semantic_error(5, node->line_number, "");
                        return NULL;
                    }
                }
                else{
                    semantic_error(6, node->line_number, "");
                    return NULL;
                }
            }
        }
        else if(strcmp(node->children[0]->name, "LP") == 0){
            // Exp -> LP Exp RP
            return Exp(node->children[1]);
        }
        else if(strcmp(node->children[1]->name, "DOT") == 0){
            // Exp -> Exp DOT ID
            Type* type = Exp(node->children[0]);
            if(type == NULL || type->kind != STRUCTURE)
                semantic_error(13, node->line_number, "");
            else{
                string name = node->children[2]->value;
                auto iterator = symbol_table.find(name);
                if(iterator == symbol_table.end()){
                    semantic_error(14, node->line_number, name);
                    return NULL;
                }
                else return iterator->second.type;
            }
        }
        else if(strcmp(node->children[0]->name, "ID") == 0){
            // Exp -> ID LP RP
            string name = node->children[0]->value;
            auto iterator = symbol_table.find(name);
            if(iterator == symbol_table.end()){
                semantic_error(2, node->line_number, name);
                return NULL;
            }
            else{
                if(iterator->second.type->kind != FUNCTION){
                    semantic_error(11, node->line_number, name);
                    return NULL;
                }
                else if(iterator->second.type->u.function.all_arg_vector->size() != 0){
                    semantic_error(9, node->line_number, name);
                    return NULL;
                }
                return iterator->second.type->u.function.return_type;
            }
        }
    }
    else if(node->children_index == 2){
        // Exp -> MINUS Exp
        //     |  NOT Exp
        if(strcmp(node->children[0]->name, "MINUS") == 0)
            return Exp(node->children[1]);
        else if(strcmp(node->children[0]->name, "NOT") == 0){
            Type* type = Exp(node->children[1]);
            if(type == NULL)    // 这里的错误之前已经发现了
                return NULL;
            if(type->u.basic != BASIC_INT_TYPE){
                semantic_error(7, node->line_number, "");
                return NULL;
            }
            else
                return int_type_needed;
        }
    }
    else if(node->children_index == 4){
        // Exp -> ID LP Args RP
        if(strcmp(node->children[0]->name, "ID") == 0){
            string name = node->children[0]->value;
            auto iterator = symbol_table.find(name);
            if(iterator == symbol_table.end()){
                semantic_error(2, node->line_number, name);
                return NULL;
            }
            else{
                if(iterator->second.type->kind != FUNCTION){
                    semantic_error(11, node->line_number, name);
                    return NULL;
                }
                else if(node->children_index == 4){
                    vector<Type*> args = Args(node->children[2]);
                    vector<Symbol_Table_Item> declare_args = *(iterator->second.type->u.function.all_arg_vector);
                    if(args.size() != declare_args.size()){
                        semantic_error(9, node->line_number, name);
                        return NULL;
                    }
                    for(int i = 0; i < args.size(); i++){
                        if(!type_equal(args[i], declare_args[i].type)){
                            semantic_error(9, node->line_number, name);
                            return NULL;
                        }
                    }
                }
                return iterator->second.type->u.function.return_type;
            }
        }
        else if(strcmp(node->children[1]->name, "LB") == 0){
            // Exp -> Exp LB Exp RB
            Type* type = Exp(node->children[0]);
            if(type == NULL || type->kind != ARRAY){
                semantic_error(10, node->line_number, node->children[0]->children[0]->value);
                return NULL;
            }
            else if(!type_equal(Exp(node->children[2]), int_type_needed)){
                semantic_error(12, node->line_number, node->children[2]->children[0]->value);
                return NULL;
            }
            return type->u.array.element_type;
        }
    }
}

vector<Type*> Args(Node* args_node){
    // Args -> Exp COMMA Args
    //      |  Exp
    vector<Type*> all_args;
    Node* temp = args_node;
    while(true){
        all_args.push_back(Exp(temp->children[0]));
        if(temp->children_index == 3)
            temp = temp->children[2];
        else
            break;
    }
    return all_args;
}