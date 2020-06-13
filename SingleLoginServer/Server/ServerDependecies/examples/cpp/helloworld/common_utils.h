#ifndef COMMON_UTILS_INCLUDED
#define COMMON_UTILS_INCLUDED

#include <memory>
#include <string>
#include <stdexcept>

using namespace std;

// 工具函数，判断char指针是不是空字符串
bool isCharPointerEmpty(const char* str);

// 工具函数，用printf那样的方式格式化字符串
string formatStringOne(string origin, string param);
string formatStringTwo(string origin, string param1, string parma2);
string formatStringThree(string origin, string param1, string parma2, string param3);

// 工具函数，获取当前运行的bin文件的所在目录
string getSelfParentDirectory();
// 工具函数，将文件内容读取到一个string里并返回，注意，不要用来读大文件
string loadFileToString(string filePath);
// 工具函数，判断文件是否存在
bool file_exist (const std::string& name);

// 防止sql注入的过滤工具函数
// 返回false的话说明有注入风险
bool checkSQLParameter(string parameter);

// 拼接两个路径，返回拼接后的路径
string pathAppend(const string& p1, const string& p2);

// 工具函数，将const char* 转换成 std::string
string constCharPointerToString(const char* str);


#endif // COMMON_UTILS_INCLUDED