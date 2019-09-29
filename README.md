# AndroidLoginExampleWithServer
A android login example with the corresponding server code and server docker file

## 整体功能
- 客户端具备登录、注册功能
- 已经登录的客户端，如果别人在其他机器上再次登录，已登录客户端会被踢下线
- 服务端保存了，已经注册的用户信息和已经登录的用户信息，并提供接口给客户端进行登录、注册、查询是否已被踢下线
- android客户端用的java编写，后台服务器用的c++编写

## 使用到的工具
- 客户端和服务端都是用的grpc库来实现基于http2 + tls的rpc调用
- 客户端使用cmake + ndk编译本地仓库，gradle来编译apk
- 客户端的native库的jni层代码用的djinni来实现
- 服务端使用google的bazel来编译
- 服务端用的数据存储用的是mysql，mysql的c++层接口使用的是mysql++库
- 服务端的编译用的是一个ubuntu docker容器
- 使用的docker compose来启动服务器

## 代码结构
### 客户端
- 客户端的java层核心代码在clientCode/AndroidLoginClient/app/src/main/java文件夹里面
- 客户端的native层核心代码基本都在clientCode/AndroidLoginClient/app/src/main/cpp/grpc_server_message_getter.cpp这一个文件里面

### 服务端
- 服务端的核心代码都在serverCode/SingleLoginServer/examples/cpp/helloworld/greeter_server.cc这一个文件里面

## 编译 于 运行
### 客户端
- 暂时只支持在mac or linux上编译
- 将clientCode/AndroidLoginClient文件夹导入到Android Studio里面
- 给Android Studio配置好android sdk和ndk的路径
- 电脑本地的/usr/local/bin/protoc文件和/usr/local/bin/grpc_cpp_plugin要存在，如果不存在可以通过brew install grpc protobuf来安装
- 然后就可以在Android Studio上编译执行了
- 建议在Android本地模拟器上运行本demo，并且根据你用的模拟器的本地转发ip配置一条host，比如我用的腾讯手游助手host是这样的：10.0.3.2        www.myserver.com，域名在java层代码里可以配置，目前配置的是www.myserver.com

### 服务端
- 服务端是在





