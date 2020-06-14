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

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/md5.h>

#include <grpcpp/grpcpp.h>
#include <mysql++/mysql++.h>
#include <bcrypt/BCrypt.hpp>
#include <md5Utils.h>
#include <piggy_base64.h>
#include <openssl_rsa.h>

#include "tinyxml2.h"
#include "error_code.h"
#include "db_utils.h"
#include "common_utils.h"
#include "mysqlpp_query_utils.h"
#include "config_file_load_utils.h"
#include "uuid_utils.h"
#include "log_file_utils.h"

#ifdef BAZEL_BUILD
#include "examples/protos/piggy.grpc.pb.h"
#else
#include "piggy.grpc.pb.h"
#include "../../../mysqlpp/include/mysql++/result.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using piggy::HelloRequest;
using piggy::HelloReply;
using piggy::RegisterReply;
using piggy::RegisterRequest;
using piggy::Greeter;

using namespace tinyxml2;
using namespace piggy;
using namespace mysqlpp;
using namespace std;
using namespace std::chrono;
 
// RSA解密用私钥限定长度
#define DEFAULT_DECRYPT_PRIVATE_KEY_LEN 1674

// 配置文件路径，运行时传进来的参数，详细到文件名
string config_file_path;
// 日志文件保存的文件夹，最后不能有'/'
string log_file_directory;
// 服务端配置的证书文件
string servercert;
// 服务端证书对应的私玥
string serverkey;
// 服务端rsa解密用到的私玥问题
unsigned char server_private_key[DEFAULT_DECRYPT_PRIVATE_KEY_LEN];
// 服务端用的数据库的主数据库名
string single_login_server_database_name;
// 服务端用的数据库的连接ip
string single_login_server_database_ip;
// 服务端用的数据库的连接用户名
string single_login_server_database_username;
// 服务端用的数据库的连接用户密码
string single_login_server_database_password;
// 服务端监听的ip和端口
string single_login_server_ip_and_port;

// rsa解密工具函数
std::string getDecryptedPwd(const std::string user_password) {
    if (user_password.empty()) {
        return "";
    }

    std::string bbb = base64_decode(user_password);

    // 将加密后的string转回unsigned char*
    unsigned char* abcd = (unsigned char*)(bbb.c_str());
    unsigned char decrypted[2048]={};
    int decrypted_length = private_decrypt(abcd, 256, server_private_key, decrypted);

    std::string encrypted_string(decrypted, decrypted + decrypted_length);

    return encrypted_string;
}

// 登录时，检查用户名和密码是否ok
int checkUserNameAndPassword(string user_name, string user_password) {
    if (user_name.empty()) {
        loggerE("Check user name and password, user name empty!");
        return ERR_CODE_USER_NAME_EMPTY;
    }

    if (user_password.empty()) {
        loggerE("Check user name and password, user password empty!");
        return ERR_CODE_USER_PSW_EMPTY;
    }

    // 判断用户是否已经存在
    if (!isUserExistInDB(single_login_server_database_name,
                         single_login_server_database_ip,
                         single_login_server_database_username,
                         single_login_server_database_password,
                         user_name)) {
        loggerE("Check user name and password, user not exist in db!");
        return ERR_CODE_USER_PSW_OR_USER_NAME_ERROR;
    }

    // 判断用户名密码是否存在于数据库，存在则说明登录成功
    if (isUserAndPasswordExistInDB(single_login_server_database_name,
                                   single_login_server_database_ip,
                                   single_login_server_database_username,
                                   single_login_server_database_password,
                                   user_name, 
                                   user_password)) {
        loggerD("Check user name and password, user name and password exist in db!");
        return ERR_CODE_OK;
    }

    // 判断用户名密码不否存在于数据库，登录失败
    loggerE("Check user name and password, user name and password not exist in db!");
    return ERR_CODE_USER_PSW_OR_USER_NAME_ERROR;
}

// GRPC Service的实现类
class GreeterServiceImpl final : public Greeter::Service {
    // 暴露给客户端的测试用接口
    Status SayHello(ServerContext *context, const HelloRequest *request, HelloReply *reply) override {
        // 开始新业务，更新日志的session id
        updateSessionID();

        std::string prefix("Hello Response!");
        reply->set_message(prefix + request->name());
        return Status::OK;
    }

    // 暴露给客户端的注册接口
    Status TryRegister(ServerContext *context, const RegisterRequest *request, RegisterReply *reply) override {
        // 开始新业务，更新日志的session id
        updateSessionID();

        std::string user_name = request->user_name();
        std::string user_password = request->user_password();

        loggerD( "Try register, user name =  " + user_name);

        // 防止sql注入过滤
        if (!checkSQLParameter(user_name)) {
            loggerE("Try register, user name malformed = " + user_name);
            reply->set_message(std::to_string(ERR_CODE_USER_NAME_OR_PASSWORD_MALFORMED));
            return Status::OK;
        }

        // RSA解密后的用户密码
        std::string dpt_pwd = getDecryptedPwd(user_password);

        // 防止sql注入过滤
        if (!checkSQLParameter(dpt_pwd)) {
            loggerE("Try register, user password malformed!");
            reply->set_message(std::to_string(ERR_CODE_USER_NAME_OR_PASSWORD_MALFORMED));
            return Status::OK;
        }

        // 讲用户账号和密码保存到数据库，数据库里只保存bcrypt后的密码
        int ret = saveUserInfoToDB(single_login_server_database_name,
                                   single_login_server_database_ip,
                                   single_login_server_database_username,
                                   single_login_server_database_password,
                                   user_name,
                                   dpt_pwd);
        // 返回成功给客户端
        reply->set_message(std::to_string(ret));
        return Status::OK;
    }

    // 暴露给客户端的登录接口
    Status TryLogin(ServerContext *context, const LoginRequest *request, LoginReply *reply) override {
        // 开始新业务，更新日志的session id
        updateSessionID();

        string user_name = request->user_name();
        loggerD( "Try login, user name =  " + user_name);
        string user_password = request->user_password();

        string user_real_password = getDecryptedPwd(user_password);

        // 防止sql注入过滤
        if (!checkSQLParameter(user_name)) {
            loggerE("Try login, user name malformed = " + user_name);
            reply->set_message(std::to_string(ERR_CODE_USER_NAME_OR_PASSWORD_MALFORMED));
            return Status::OK;
        }

        // 防止sql注入过滤
        if (!checkSQLParameter(user_real_password)) {
            loggerE("Try login, user password malformed!");
            reply->set_message(std::to_string(ERR_CODE_USER_NAME_OR_PASSWORD_MALFORMED));
            return Status::OK;
        }

        // 判断用户名和密码是否存在数据库里，存在的话则开始生成登录token
        int ret = checkUserNameAndPassword(request->user_name(), user_real_password);

        if (ret != ERR_CODE_OK) {
            loggerE("Try login, user name and password not in db!");
            reply->set_message(std::to_string(ret));
            reply->set_token("");
            return Status::OK;
        }

        // 获取生成token需要用到的时间戳
        milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
        string current_time = std::to_string(ms.count());
        loggerD("Try login, generate login token, time milli =  " + current_time);

        // 获取生成token需要用到的固定8位长的随机数
        int max = 99999999, min = 10000000, range; 
	    range = max - min + 1;
	    int randomInFixRan = rand() % range + min;
        loggerD("Try login, generate login token, random num =  " + to_string(randomInFixRan));

        // 添加随机数，防止被猜到生成规则，被恶意碰撞
        class MD5 md5Object2(request->user_name() + current_time + std::to_string(randomInFixRan));
        string token = md5Object2.toStr();
        loggerD("Try login, token = " + token);

        // 将用户的token保存起来
        saveUserTokenToDB(single_login_server_database_name,
                          single_login_server_database_ip,
                          single_login_server_database_username,
                          single_login_server_database_password,
                          user_name, 
                          token);

        reply->set_message(std::to_string(ret));
        reply->set_token(token);
        return Status::OK;
    }

    // 暴露给客户端登录token状态查询接口
    Status CheckKickOut(ServerContext *context, const CheckKickOutRequest *request, CheckKickOutReply *reply) override {
        // 开始新业务，更新日志的session id
        updateSessionID();

        string user_name = request->user_name();
        string user_token = request->user_token();

        loggerD("Check kick out, user name =  " + user_name);
        loggerD("Check kick out, user token =  " + user_token);

        // 防止sql注入过滤
        if (!checkSQLParameter(user_name)) {
            loggerE("Check kick out, user name malformed = " + user_name);
            reply->set_message(std::to_string(ERR_CODE_USER_NAME_OR_PASSWORD_MALFORMED));
            return Status::OK;
        }

        // 防止sql注入过滤
        if (!checkSQLParameter(user_token)) {
            loggerE("Try kick out, user token malformed = " + user_token);
            reply->set_message(std::to_string(ERR_CODE_USER_NAME_TOKEN_MALFORMED));
            return Status::OK;
        }

        // 检查用户名和token是否存在于数据库，存在的话则认为token有效
        if (isUserTokenExistInDB(single_login_server_database_name,
                                 single_login_server_database_ip,
                                 single_login_server_database_username,
                                 single_login_server_database_password,
                                 user_name, 
                                 user_token)) {
            reply->set_message(std::to_string(ERR_CODE_OK));
        } else {
            reply->set_message(std::to_string(ERR_CODE_TOKEN_OUTDATED));
        }

        return Status::OK;
    }
};

// 检查并加载配置文件
bool initServer() {
    // 初始化日志文件夹
    init_log_file(log_file_directory);
    // 设置日志文件的初始session id
    updateSessionID();

    // 尝试加载配置文件，并获取加载结果
    ConfigFileLoadResult loadConfigResult = loadConfigFile(config_file_path);
    if (!loadConfigResult.loadResultAllOK) {
        loggerE("Init server, load config file not all field ok!");
        return false;
    }

    // 读取到的配置保存起来
    servercert = loadConfigResult.ServerCert;
    serverkey = loadConfigResult.ServerCertPrivateKey;
    // 校验读取到的服务器解密用私钥的长度
    int keyLen = loadConfigResult.ServerDecryptPrivateKey.length();
    if (keyLen != DEFAULT_DECRYPT_PRIVATE_KEY_LEN) {
        loggerE("Init server, load config file decrypt key len not match = " +  DEFAULT_DECRYPT_PRIVATE_KEY_LEN);
        return false;
    }
    strcpy((char *)server_private_key, loadConfigResult.ServerDecryptPrivateKey.c_str());
    single_login_server_database_name = loadConfigResult.ServerDatabaseName;
    single_login_server_database_ip = loadConfigResult.ServerDatabaseIP;
    single_login_server_database_username = loadConfigResult.ServerDatabaseUserName;
    single_login_server_database_password = loadConfigResult.ServerDatabaseUserPassword;
    single_login_server_ip_and_port = loadConfigResult.ServerIPAndPort;

    // 打印配置加载的结果
    loggerD("Init server, load config file final result servercert = " + servercert);
    loggerD("Init server, load config file final result serverkey = " + serverkey);
    loggerD("Init server, load config file final result server_private_key = " + string(reinterpret_cast<char*>(server_private_key), 4));
    loggerD("Init server, load config file final result single_login_server_database_name = " + single_login_server_database_name);
    loggerD("Init server, load config file final result single_login_server_database_ip = " + single_login_server_database_ip);
    loggerD("Init server, load config file final result single_login_server_database_username = " + single_login_server_database_username);
    loggerD("Init server, load config file final result single_login_server_database_password = " + single_login_server_database_password);
    loggerD("Init server, load config file final result single_login_server_ip_and_port = " + single_login_server_ip_and_port);

    return true;
}

// 启动GRPC Server并一直监听特定端口的消息
void RunServer() {
    // 配置GRPC服务器的https证书
    grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp;
    pkcp.private_key = serverkey;
    pkcp.cert_chain = servercert;
    grpc::SslServerCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs="";
    ssl_opts.pem_key_cert_pairs.push_back(pkcp);
    std::shared_ptr<grpc::ServerCredentials> creds;
    creds = grpc::SslServerCredentials(ssl_opts);

    GreeterServiceImpl service;

    ServerBuilder builder;
    // 设置服务器监听的ip和端口
    builder.AddListeningPort(single_login_server_ip_and_port, creds);
    // 10秒一个ping包
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 10000);
    // 20秒的ping包超时时间
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 20000);
    // 没有请求的情况下，也允许无限发送ping包
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MAX_PINGS_WITHOUT_DATA, 0);
    // 没有请求也允许发送ping包
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, true);
    // 允许无限多的bad ping
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MAX_PING_STRIKES, 0);
    // 服务端允许的最短ping间隔
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_RECV_PING_INTERVAL_WITHOUT_DATA_MS, 5000);
    // 最短ping包间隔
    builder.AddChannelArgument(GRPC_ARG_HTTP2_MIN_SENT_PING_INTERVAL_WITHOUT_DATA_MS, 5000);

    // 注册Server实现类对象
    builder.RegisterService(&service);
    // 构建GRPC server
    unique_ptr <Server> server(builder.BuildAndStart());
    loggerD("Server listening on " + single_login_server_ip_and_port);

    // 启动GRPC server并无限循环监听指定ip和端口传入的消息
    server->Wait();
}

// 整个程序的入口函数，需要传入两个参数
// 第一个是指定的配置文件的完整路径
// 第二个是指定的日志生成目录
int main(int argc, char **argv) {
    // 启动参数不是两个的话直接退出，3包含启动命令本身，实际参数是两个
    if (argc != 3) {
        cout << "Argument num not 2, current is " << argc << std::endl;
        return -1;
    }

    // 获取配置文件路径
    config_file_path = argv[1];
    cout << "Argument config file path = " << config_file_path << std::endl;
    // 获取日志文件夹
    log_file_directory = argv[2];
    cout << "Argument log file directory = " << log_file_directory << std::endl;

    // 初始化服务器，主要是加载配置文件，初始化日志文件
    bool initResult = initServer();

    // 加载配置文件失败则立即退出
    if (!initResult) {
        loggerE("Server init fail, please check config file exist and valid!");
        return 0;
    } else {
        loggerD("Server init success!");
    }

    // 启动GRPC Server
    RunServer();

    return 0;
}
