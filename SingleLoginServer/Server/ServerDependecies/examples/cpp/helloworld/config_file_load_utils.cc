#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <codecvt>

#include "common_utils.h"
#include "config_file_load_utils.h"
#include "JSON.h"
#include "log_file_utils.h"

using namespace std;

// 工具函数，负责将wstring转换成string
string wstringToString(const wstring param) {
    if (param.empty()) {
        return "";
    }

    // 准备转换器
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    // 转换并返回
    return converter.to_bytes(param);
}

// 工具函数，传入json跟节点对象和字段名，返回改字段对应的值
string parseJsonStringElement(JSONObject root, wchar_t* elementName) {
    if (elementName == nullptr) {
        return "";
    }

    if (root.find(elementName) != root.end() && root[elementName]->IsString()) {
        wstring result = root[elementName]->AsString();
        return wstringToString(result);
    }

    return "";
}

// 工具函数，负责加载配置文件并返回加载成功后的所有配置
ConfigFileLoadResult loadConfigFile(string configFilePath) {

    loggerD("Load config file, file path = " + configFilePath);

    ConfigFileLoadResult result;

    // 将文件加载成string形式
    string configFileContent = loadFileToString(configFilePath);
    loggerD("Load config file, config file content = " + configFileContent);
    if (configFileContent.empty()) {
        loggerE("Load config file, config file content empty!");
        return result;
    }

    // 尝试解析json字符串内容
    JSONValue *value = JSON::Parse(configFileContent.c_str());
    if (value == nullptr || !value->IsObject()) {
        loggerE("Load config file, config file parse error!");
        return result;
    } else {
        loggerD("Load config file, config file parse ok!");
    }

    JSONObject root = value->AsObject();

    // 加载服务器证书 
    string serverCertPath = parseJsonStringElement(root, L"ServerCertPath");
    loggerD("Load config file, serverCertPath = " + serverCertPath);
    result.ServerCert = loadFileToString(serverCertPath);
    loggerD("Load config file, serverCert  = " + result.ServerCert);

    // 加载服务器证书对应的私钥 
    string serverCertPrivateKeyPath = parseJsonStringElement(root, L"ServerCertPrivateKeyPath");
    loggerD("Load config file, serverCertPrivateKeyPath = " + serverCertPrivateKeyPath);
    result.ServerCertPrivateKey = loadFileToString(serverCertPrivateKeyPath);

    // 加载服务器解密用私钥 
    string serverDecryptPrivateKeyPath = parseJsonStringElement(root, L"ServerDecryptPrivateKeyPath");
    loggerD("Load config file, serverDecryptPrivateKeyPath = " + serverDecryptPrivateKeyPath);
    result.ServerDecryptPrivateKey = loadFileToString(serverDecryptPrivateKeyPath);
    
    // 加载数据库名
    result.ServerDatabaseName = parseJsonStringElement(root, L"ServerDatabaseName");
    loggerD("Load config file, ServerDatabaseName = " + result.ServerDatabaseName);
    
    // 加载数据库连接ip
    result.ServerDatabaseIP = parseJsonStringElement(root, L"ServerDatabaseIP");
    loggerD("Load config file, ServerDatabaseIP = " + result.ServerDatabaseIP);
    
    // 加载数据库连接用户名
    result.ServerDatabaseUserName = parseJsonStringElement(root, L"ServerDatabaseUserName");
    loggerD("Load config file, ServerDatabaseUserName = " + result.ServerDatabaseUserName);
    
    // 加载数据库连接密码
    result.ServerDatabaseUserPassword = parseJsonStringElement(root, L"ServerDatabaseUserPassword");
    loggerD("Load config file, ServerDatabaseUserPassword = " + result.ServerDatabaseUserPassword);
    
    // 加载服务器ip和端口
    result.ServerIPAndPort = parseJsonStringElement(root, L"ServerIPAndPort");
    loggerD("Load config file, ServerIPAndPort = " + result.ServerIPAndPort);

    // 所有字段都加载到内容，才算整体加载成功
    result.loadResultAllOK = !result.ServerCert.empty() &&
                             !result.ServerCertPrivateKey.empty() &&
                             !result.ServerDecryptPrivateKey.empty() &&
                             !result.ServerDatabaseName.empty() &&
                             !result.ServerDatabaseIP.empty() &&
                             !result.ServerDatabaseUserName.empty() &&
                             !result.ServerDatabaseUserPassword.empty() &&
                             !result.ServerIPAndPort.empty();

    return result;

    
}