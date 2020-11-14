#include <iostream>
#include <string>
#include <fstream>
#include <vector>

extern "C" int yylex(void);

using namespace std;


int main(int argc, char* argv[]) {
    vector<string> vec;          // 好像必须用个vector
    ifstream file(argv[1]);
    string temp;
    if (!file.is_open()) {
        cout << "未成功打开文件" << endl;
    }
    while (getline(file, temp)) {
        vec.push_back(temp);
    }
    for (auto it = vec.begin(); it != vec.end(); it++) {
        cout << *it << endl;
    }
    return 0;


}
