#ifndef LOG_FILE_UTILS_INCLUDED
#define LOG_FILE_UTILS_INCLUDED

#include <iostream>

using namespace std;

// 工具函数，初始化日志文件夹
void init_log_file(string logFilePath);
// 更新当前日志文件的SessionID
void updateSessionID();
// 打印Debug级别日志
void loggerD(string logMsg);
// 打印Error级别日志
void loggerE(string logMsg);

#endif // LOG_FILE_UTILS_INCLUDED

