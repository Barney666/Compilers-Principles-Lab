#include <iostream>
#include <string>
#include <fstream>

using namespace std;

/**
 *  实现cat的基本功能
 *
 *  argc和argv参数在用命令行编译程序时有用。main( int argc, char* argv[], char **env ) 中
 *  第一个参数，int型的argc，用来统计程序运行时发送给main函数的命令行参数的个数，在VS中默认值为1。
 *  第二个参数，char*型的argv[]，为字符串数组，用来存放指向的字符串参数的指针数组，每一个元素指向一个参数。各成员含义如下：
 *  argv[0]指向程序运行的全路径名，argv[1]指向在DOS命令行中执行程序名后的第一个字符串，argv[2]指向命令行中执行程序名后的第二个字符串...
 * */

int main(int argc, char* argv[]) {
    ifstream file(argv[1]);
    string temp;
    if (!file.is_open()) {
        cout << "未成功打开文件" << endl;
    }
    while (getline(file, temp)) {     // 每次输出string可以，char就不行，不知道为啥
        cout << temp << endl;
    }
    return 0;
}
