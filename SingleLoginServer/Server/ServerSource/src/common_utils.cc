#include <stdio.h>
#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include <limits.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <ctype.h>
#include <chrono>

#include "common_utils.h"
#include "log_file_utils.h"

using namespace std;

// 工具函数，判断char指针是不是空字符串
bool isCharPointerEmpty(const char* str) {
    if (str == NULL) {
        return true;
    }

    return str[0] == '\0';
}

// 工具函数，用printf那样的方式格式化字符串
string formatStringOne(string origin, string param) {
    if (origin.empty()) {
        return "";
    }

    if (param.empty()) {
        return origin;
    }

    char buff[origin.length() + param.length()];
    snprintf(buff, sizeof(buff), origin.c_str(), param.c_str());
    std::string buffAsStdStr = buff;
    return buffAsStdStr;
}  

// 工具函数，用printf那样的方式格式化字符串
string formatStringTwo(string origin, string param1, string param2) {
    if (origin.empty()) {
        return "";
    }

    if (param1.empty()) {
        return origin;
    }

    if (param2.empty()) {
        return origin;
    }

    char buff[origin.length() + param1.length() + param2.length()];
    snprintf(buff, sizeof(buff), origin.c_str(), param1.c_str(), param2.c_str());
    std::string buffAsStdStr = buff;
    return buffAsStdStr;
}  

// 工具函数，用printf那样的方式格式化字符串
string formatStringThree(string origin, string param1, string param2, string param3) {
    if (origin.empty()) {
        return "";
    }

    if (param1.empty()) {
        return origin;
    }

    if (param2.empty()) {
        return origin;
    }

    if (param3.empty()) {
        return origin;
    }

    char buff[origin.length() + param1.length() + param2.length() + param3.length()];
    snprintf(buff, sizeof(buff), origin.c_str(), param1.c_str(), param2.c_str(), param3.c_str());
    std::string buffAsStdStr = buff;
    return buffAsStdStr;
}  

// 工具函数，将文件内容读取到一个string里并返回，注意，不要用来读大文件
string loadFileToString(string filePath) {
    loggerD("Load file to string, file path = " + filePath);

    if (filePath.empty()) {
        loggerE("Load file to string, file path empty!");
        return "";
    }

    if (!file_exist(filePath)) {
        loggerE("Load file to string, file not exist!");
        return "";
    }

    // 读取文件内容
    std::ifstream t(filePath);
    std::stringstream buffer;
    buffer << t.rdbuf();

    loggerD("Load file to string, file content = " + buffer.str());
    // 文件内容以string方式返回
    return buffer.str();
}

// 工具函数，判断文件是否存在
bool file_exist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0); 
}

// 防止sql注入的过滤工具函数
// 返回false的话说明有注入风险
bool checkSQLParameter(string parameter) {
    loggerD("Check SQL parameter str = " + parameter);
    string key[14] = {"and","*","="," ","%0a","%","/","union","|","&","^" ,"#","/*","*/" };
    for (int i = 0; i < 14; i++) {
        if (parameter.find(key[i]) != string::npos) {   
            loggerD("Check SQL parameter str = " + parameter + ", illegal part found = " + key[i]);
            return false;
        }
    }

    loggerD("Check SQL parameter str = " + parameter + ", check passed!");
    return true;
}

// 拼接两个路径，返回拼接后的路径
string pathAppend(const string& p1, const string& p2) {

   char sep = '/';
   string tmp = p1;

#ifdef _WIN32
  sep = '\\';
#endif

  if (p1[p1.length()] != sep) { // Need to add a
     tmp += sep;                // path separator
     return (tmp + p2);
  }
  else
     return (p1 + p2);
}

// 工具函数，将const char* 转换成 std::string
string constCharPointerToString(const char* str) {
    if (str == nullptr) return "";

    std::string result(str);
    return result;
}
  