#include <atomic>

#include <grpc++/grpc++.h>
#include <jni.h>
#include <android/log.h>

#include "openssl_rsa.h"

#include "piggy_base64.h"

#include "piggy.grpc.pb.h"

#include <string>
#include <server_message_getter.hpp>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using piggy::Greeter;
using piggy::HelloReply;
using piggy::HelloRequest;

using namespace piggy;

std::atomic<bool> stop_server(false);

#define LOG_TAG "Faip"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// 服务器自签证书文件
std::string servercert = "-----BEGIN CERTIFICATE-----\n"
                         "MIIFUjCCAzoCAQEwDQYJKoZIhvcNAQELBQAwbzELMAkGA1UEBhMCVVMxCzAJBgNV\n"
                         "BAgMAkNBMRUwEwYDVQQHDAxTYW5GcmFuY2lzY28xDzANBgNVBAoMBkdvb2dsZTEQ\n"
                         "MA4GA1UECwwHeW91dHViZTEZMBcGA1UEAwwQd3d3Lm15c2VydmVyLmNvbTAeFw0x\n"
                         "OTA5MjUxOTA4MDlaFw0yMDA5MjQxOTA4MDlaMG8xCzAJBgNVBAYTAlVTMQswCQYD\n"
                         "VQQIDAJDQTEVMBMGA1UEBwwMU2FuRnJhbmNpc2NvMQ8wDQYDVQQKDAZHb29nbGUx\n"
                         "EDAOBgNVBAsMB3lvdXR1YmUxGTAXBgNVBAMMEHd3dy5teXNlcnZlci5jb20wggIi\n"
                         "MA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQDUR4x8u/w6wmIs9BHsWbAWcZyW\n"
                         "nacQdxZh+ko1z7d26ybkfwUrw3KSeuwntEsOWNKrkt0ZvNs4OTDFshbEgkr6mkmp\n"
                         "PTgd9PAZ0os4Hu7EMBy2E9el8XEt9FC/aqgbg9Mvj68s2Xc7I2BcgmIt6BQ48FRH\n"
                         "54gVGf6vH6WFiSVStI4Nwcrhnf/EtPVcQwRHGYQu7jPnYBScDrz3w4Gnzxrexo35\n"
                         "PqEcqZAVcQMEHASjqU6sDNhdLPzVoyRMpkofe6ykuE5E1qqbWGo7B+Rf7BfrFQ1R\n"
                         "Tu8PYeOfN9GE46tnAAscsdAX/5AOL+kgx3ZCcMefJpizJOWxa8cG2dfOhiBTsfzu\n"
                         "ipIqdR+kfLXJ4UcstegDsmr86vuP6KCCICidsjNK3WBX67w5huIDcnBx1LsReUhm\n"
                         "9oFk+u2GC5uwnE9ksyubUJaJ+kKSh5Jnl7GFhtXX0C/ziM+CE1hEy1tXKMk+n0QK\n"
                         "OJXAV/nxXCqVrm6oYJN4uSAKSzUJF8hzEyK4zm5h7sLjiYPvy7DMYpYNlPC4wHM7\n"
                         "iYVDxKcE/VBOX1VjjNqmwlQYyb0D2jmXlJ9Es0DpUwMFmvZjVsatdarAcKl/I0gq\n"
                         "6kKYofx8NMV9wP9rVhLnbLc8AKmC8TQl18SwYRbamJqbzyNFaHd/zDqWRRQP4cOi\n"
                         "oWmJ57qQ2vUXEvWtRQIDAQABMA0GCSqGSIb3DQEBCwUAA4ICAQDT45/aFYUvd28v\n"
                         "vHnw23bObJdF0X7l28ecB6W9WAt2gKBWQr0S2uhW0qfmUsVZ96h4x8KyPF0YCDD4\n"
                         "tz2SL+Z84WiUZbg1uiY2FZsAjJTb+2lsWbJWSudSMCvyElDnMw6Wv5W1nIFBdE5f\n"
                         "+GaVWrv7dNW8uAUaBt3CtmYZ08EAtB/rtAMLUe+ufi0dh0BVST5UM8XZK/66kcV/\n"
                         "9QSwclrV4FH+Hu8bJxwf4cOqBSThe0AZtiH8quCTyDgf0CdNNywWSVwyDFBng7/4\n"
                         "PZNAHNoWFe6Pa/nVmR0TvSgxIZwTWGTEnH5qT9yv/D5nY3eHhgDD77lDjA5Dg2ib\n"
                         "OEleXojDY0/0bQiqiRmJUbaPmNfbinmGLzWniAXeLHX4GfZkiRQx/STJ9bbdH3lu\n"
                         "QsBrYRNsmwby4+Lrlk6OkauVq32qwznYQtnMkS51+LdUf+QOrQJr9maTm0AbQtsS\n"
                         "y4mBMU44utP+yKq4GKO5UYbrWC/V+MmyHDdNMKv8nHndwcnoEEHJ+J0RAvN/fJdw\n"
                         "YV5Vbf/0S1qmybYNizgdKT3tKpvzDHLyPTV2ujlGVv1I9D0tVu5Eh2jE4vL9mQaO\n"
                         "+1yL1PeHgVBfi2Km4nRr/Xdp20G3l2eyrR7nOBZ2SHKHVre+OXxY4emkEWkMmLAO\n"
                         "lBEws9a1sDU+m49mH4/NX7qanxiTCw==\n"
                         "-----END CERTIFICATE-----";

// RSA加密用的公钥
unsigned char client_public_key[] = "-----BEGIN PUBLIC KEY-----\n"
                                "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuC24gUsxt7HggDw1UPTE\n"
                                "qekujh1Snd2h63j/+48ao6W4DYn1sAKFrjJWkeOiNsKJZmpFtPGnqxxXooyRSl1d\n"
                                "iMLVLkZqBLLd7xdqpr8J2e4IqzsUVdxZuNvROKYPpVM0AQwFf8Xd2qneWTkhM+PD\n"
                                "dH4JM2Z+0G1LkyHO3g1ACJnUZqPgUrzuOxBVHMsQvmx94vdFmUldebwPAG1aPW42\n"
                                "eJ29kxvvYDxRhFTEC7P6Rq5Rf+6Jdgrj8WnP/iculGVb109PyuvfiI6KRjMh8DbD\n"
                                "2l0YRIU5ua9uG4wp7BKLzpEaxsx95MKDK1iUHL8sj9lh0ArtFDHoigVWb1FA4BQP\n"
                                "cQIDAQAB\n"
                                "-----END PUBLIC KEY-----";


// grpc client端接口封装类
class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
      : stub_(Greeter::NewStub(channel)) {}

  // 测试用接口
  std::string SayHello(const std::string& user) {
    // Data we are sending to the server.
    HelloRequest request;
    request.set_name(user);

    // Container for the data we expect from the server.
    HelloReply reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;
    // The actual RPC.
    Status status = stub_->SayHello(&context, request, &reply);

    if (status.ok()) {
      return reply.message();
    } else {
      return status.error_message();
    }
  }

  // client端注册接口，封装了发起网络连接，请求服务端的注册结果的功能
  std::string TryRegister(const std::string &user, const std::string &user_password) {
      // Data we are sending to the server.
      RegisterRequest request;
      request.set_user_name(user);
      request.set_user_password(user_password);

      // Container for the data we expect from the server.
      RegisterReply reply;

      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      std::chrono::system_clock::time_point deadline =
              std::chrono::system_clock::now() + std::chrono::seconds(5);
      context.set_deadline(deadline);

      // The actual RPC.
      Status status = stub_->TryRegister(&context, request, &reply);

      std::cout << "Network code = " << status.error_code() << std::endl;
      std::cout << "Network err message = " << status.error_message() << std::endl;
      std::cout << "Network err detail = " << status.error_details() << std::endl;

      // Act upon its status.
      if (status.ok()) {
          return reply.message();
      } else {
          std::cout << status.error_code() << ": " << status.error_message() << std::endl;
          LOGD("Register fail, code = %d", status.error_code());
          LOGD("Register fail, msg = %s", status.error_message().c_str());
          return "RPC failed";
      }
  }

  // client端登录接口，封装了发起网络连接，请求服务端的登录结果的功能
  std::string TryLogin(const std::string &user, const std::string &user_password) {
      LoginRequest request;
      request.set_user_name(user);
      request.set_user_password(user_password);

      // Container for the data we expect from the server.
      LoginReply reply;

      // Context for the client. It could be used to convey extra information to
      // the server and/or tweak certain RPC behaviors.
      ClientContext context;

      std::chrono::system_clock::time_point deadline =
              std::chrono::system_clock::now() + std::chrono::seconds(5);
      context.set_deadline(deadline);

      // The actual RPC.
      Status status = stub_->TryLogin(&context, request, &reply);

      std::cout << "Network code = " << status.error_code() << std::endl;
      std::cout << "Network err message = " << status.error_message() << std::endl;
      std::cout << "Network err detail = " << status.error_details() << std::endl;

      // Act upon its status.
      if (status.ok()) {
          return reply.token();
      } else {
          std::cout << status.error_code() << ": " << status.error_message()
                    << std::endl;
          return "";
      }
  }

    // client端登录状态查询接口，封装了发起网络连接，请求服务端的登录session是否已被踢下线的功能
    std::string CheckKickOut(const std::string &user, const std::string &user_token) {

        LOGD("Native check kick out user = %s", user.c_str());
        LOGD("Native check kick out user_token = %s", user_token.c_str());

        CheckKickOutRequest request;
        request.set_user_name(user);
        request.set_user_token(user_token);

        // Container for the data we expect from the server.
        CheckKickOutReply reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        ClientContext context;

        std::chrono::system_clock::time_point deadline =
                std::chrono::system_clock::now() + std::chrono::seconds(5);
        context.set_deadline(deadline);

        // The actual RPC.
        Status status = stub_->CheckKickOut(&context, request, &reply);

        std::cout << "Network code = " << status.error_code() << std::endl;
        std::cout << "Network err message = " << status.error_message() << std::endl;
        std::cout << "Network err detail = " << status.error_details() << std::endl;

        // Act upon its status.
        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return "RPC failed";
        }
    }

 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

// 获取GreeterClient类对象的统一接口，这里会统一给GreeterClient设置ssl连接和自签证书功能
GreeterClient getGreeterClient(const std::string &host_and_port) {
    std::string cacert = servercert;
    grpc::SslCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs=cacert;

    auto ssl_creds = grpc::SslCredentials(ssl_opts);

    GreeterClient greeter(grpc::CreateChannel(host_and_port, ssl_creds));

    return greeter;
}

// 这里的ns_piggy_native命名空间是djinni声明的
namespace ns_piggy_native {

    // 输入一个字符串，返回rsa公钥加密后的字符串
    std::string getEncryptedPwd(const std::string user_password) {
        unsigned char encrypted[2048] = {};

        unsigned char* abc = (unsigned char*)(user_password.c_str());

        int encrypted_length = public_encrypt(abc, strlen((char*)abc), client_public_key, encrypted);

        // encrytped后的内容转成string
        std::string encrypted_string(encrypted, encrypted + encrypted_length);

        std::string aaa = base64_encode(encrypted_string);

        return aaa;
    }

    // 通过djinni封装后的java接口对应的native实现，负责调用登录相关逻辑
    std::string ServerMessageGetter::try_login(
        const std::string &host_and_port,
        const std::string &user_name,
        const std::string &user_password) {

        LOGD("UUUU Test pwd = %s", user_password.c_str());

        std::string ect_pwd = getEncryptedPwd(user_password);

        LOGD("UUUU Test pwd encrypted = %s", ect_pwd.c_str());

        std::string reply = getGreeterClient(host_and_port).TryLogin(user_name, ect_pwd);

        return reply;
    }

    // 通过djinni封装后的java接口对应的native实现，负责调用登录相关逻辑
    std::string ServerMessageGetter::try_register(
        const std::string &host_and_port,
        const std::string &user_name,
        const std::string &user_password) {

        std::string ect_pwd = getEncryptedPwd(user_password);

        LOGD("Register address = %s", host_and_port.c_str());
        LOGD("Register user_name = %s", user_name.c_str());
        LOGD("Register user_password = %s", user_password.c_str());

        std::string reply = getGreeterClient(host_and_port).TryRegister(user_name, ect_pwd);

        return reply;
    }

    // 通过djinni封装后的java接口对应的native实现，负责调用查询session是否被挤下线的相关逻辑
    std::string ServerMessageGetter::try_status(
        const std::string &host_and_port,
        const std::string &user_name,
        const std::string &user_token) {

        LOGD("try_check_login_status address = %s", host_and_port.c_str());
        LOGD("try_check_login_status user_name = %s", user_name.c_str());
        LOGD("try_check_login_status user_token = %s", user_token.c_str());

        std::string reply = getGreeterClient(host_and_port).CheckKickOut(user_name, user_token);

        return reply;
    }
}
