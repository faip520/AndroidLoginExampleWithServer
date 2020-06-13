#include <iostream>
#include <fstream>

#include "log_file_utils.h"
#include "uuid_utils.h"

using namespace std;

#define SERVER_LOG_FILE "Server.log" 

// 当前日志的SessionID，不更新的话，之后的所有日志都会用这个SessionID
string currentSessionID = "";
// 当前的配置文件路径
string currentLogFilePath = "";

// 更新当前日志文件的SessionID
void updateSessionID() {
    currentSessionID = generate_uuid_v4();
}

// 初始化日志文件路径，输入到文件夹就行了
void init_log_file(string logFilePath) {
    currentLogFilePath = logFilePath;
}

// 获取当前的时间，每条日志前都有时间戳
string getCurrentDateTime(string s) {
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return string(buf);
};

// 打印Debug级别日志
void loggerD(string logMsg) {
    if (currentLogFilePath.empty()) {
        cout << "Log file debug, log file path empty!" << endl;
        return;
    }

    string filePath = currentLogFilePath + "/log_" + getCurrentDateTime("date")+".txt";
    freopen(filePath.c_str(), "a+", stdout);

    string now = getCurrentDateTime("now");
    cout << "[" << now << "]" << "[" << currentSessionID << "]" << "[DEBUG]" << logMsg << endl;
}

// 打印Error级别日志
void loggerE(string logMsg) {
    if (currentLogFilePath.empty()) {
        cout << "Log file error, log file path empty!" << endl;
        return;
    }

    string filePath = currentLogFilePath + "/log_" + getCurrentDateTime("date")+".txt";
    freopen(filePath.c_str(), "a+", stdout);

    string now = getCurrentDateTime("now");
    cout << "[" << now << "]" << "[" << currentSessionID << "]" << "[ERROR]" << logMsg << endl;
}