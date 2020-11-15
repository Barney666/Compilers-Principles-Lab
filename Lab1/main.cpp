#include <iostream>
#include <string>
#include <fstream>

using namespace std;

extern "C" int lex(const char* input);     // 返回值表示是否有词法错误

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
    if(!lex(file_content.data())){
        ifstream out_file("src/out.txt");
        cerr << "";    // 没有这个下面没法输出，好离谱啊，这玩意要事先激活的？
        if(out_file.is_open()){
            while (getline(out_file, temp))
                cerr << temp << endl;
        }
        out_file.close();
    }
    return 0;
}
