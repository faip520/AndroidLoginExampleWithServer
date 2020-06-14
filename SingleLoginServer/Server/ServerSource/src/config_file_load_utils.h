#ifndef CONFIG_FILE_LOAD_UTILS_INCLUDED
#define CONFIG_FILE_LOAD_UTILS_INCLUDED

#include <iostream>

using namespace std;

// 配置文件加载时返回的结构体
typedef struct ConfigFileLoadResult {
    // 服务端配置的证书文件
    string ServerCert = "";
    // 服务端证书对应的私玥
    string ServerCertPrivateKey = "";
    // 服务端rsa解密用到的私玥问题
    string ServerDecryptPrivateKey = "";
    // 服务端用的数据库的主数据库名
    string ServerDatabaseName = "";
    // 服务端用的数据库的连接ip
    string ServerDatabaseIP = "";
    // 服务端用的数据库的连接用户名
    string ServerDatabaseUserName = "";
    // 服务端用的数据库的连接用户密码
    string ServerDatabaseUserPassword = "";
    // 服务端监听的ip和端口
    string ServerIPAndPort = "";
    // 是否全部配置都加载成功，默认是false
    bool loadResultAllOK = false;
} ConfigFileLoadResult;

// 工具函数，负责加载配置文件并返回加载成功后的所有配置
ConfigFileLoadResult loadConfigFile(string configFilePath);


#endif // CONFIG_FILE_LOAD_UTILS_INCLUDED