# 单点登录系统
包含一个单点登录客户端和服务端，在客户端A后登录账号后，在客户端B再次登录同一个账号，客户端A会被踢下线。

## 使用到的开源组件
- grpc 实现客户端服务端之间的远程方法调用
- djinnni 实现Android客户端java层和native层的胶水层代码
- openssl 实现客户端和服务端之间的非对称通讯加密
- bazel 服务端项目的编译系统
- docker 实现服务端的容器化实现
- mysql 服务端的数据库实现

## 整体功能介绍
- 客户端
  - 客户端使用Android java编写，cmake + ndk编译c++ native代码，gradle来编译整个apk
  - 已经登录的客户端，如果别人在其他机器上再次登录，已登录客户端会被踢下线
  - 客户端的native代码的jni层代码用的djinni来实现
  - 
- 服务端
  - 服务端使用C++语言开发，用Google的bazel编译系统编译
  - 服务端用的数据存储用的是mysql，mysql的c++层接口使用的是mysql++库
  - 服务端保存了已经注册的用户信息和登录token并提供了三个接口给客户端：
    - 登录、注册、查询登录态是否过期
  - 服务端的编译环和运行境用的是一个64位ubuntu docker容器
  - 服务端的通过Json配置文件动态加载所需的配置，Json配置文件路径通过启动时参数传入
  - 服务端会自动将运行日志文件输出到指定的日志目录，日志文件自动按日期区分不同文件

## 整体架构图
![Image](https://github.com/faip520/AndroidLoginExampleWithServer/blob/master/image/abc.png)

### 客户端架构图
![Image](https://github.com/faip520/AndroidLoginExampleWithServer/blob/master/image/abc.png)
### 服务端架构图
![Image](https://github.com/faip520/AndroidLoginExampleWithServer/blob/master/image/abc.png)
## 整体流程图
![Image](https://github.com/faip520/AndroidLoginExampleWithServer/blob/master/image/abc.png)

## 服务端接口文档
由于使用了grpc封装，接口请求参数和返回参数都已经被封装成Java / C++对象。

### 登录接口
请求字段        |字段类型       |字段说明
------------|-----------|-----------
user_name       |string        |用户id，只允许数字和字母的组合
user_password       |string        |用户密码，只允许数字和字母的组合

返回字段        |字段类型       |字段说明
------------|-----------|-----------
reply       |string        |返回的错误码，如果是0说明请求成功，非0则是具体的错误码
token       |string        |如果登录成功，则会返回对应的登录token

### 注册接口
请求字段        |字段类型       |字段说明
------------|-----------|-----------
user_name       |string        |用户id，只允许数字和字母的组合
user_password       |string        |用户密码，只允许数字和字母的组合

返回字段        |字段类型       |字段说明
------------|-----------|-----------
reply       |string        |返回的错误码，如果是0说明请求成功，非0则是具体的错误码


### 登录态查询接口
请求字段        |字段类型       |字段说明
------------|-----------|-----------
user_name       |string        |用户id，只允许数字和字母的组合
user_token       |string        |用户登录后拿到的登录token

返回字段        |字段类型       |字段说明
------------|-----------|-----------
reply       |string        |返回的错误码，如果是0说明请求成功，非0则是具体的错误码

## 错误码设计
状态码       |说明
------------|-----------
0       |请求成功
10001       |服务端初始化相关错误
100010000       |服务端初始化成功
100010001       |服务端配置文件不存在
100010002       |服务端配置文件格式错误
10002       |登录相关错误码
100020001       |用户id已经存在
100020002       |用户id或者密码格式有误
100020003       |用户id或者密码长度有误
100020004       |登录发生数据库错误
10003       |注册相关错误码
100030001       |用户id或者密码格式有误
100030002       |用户id或者密码长度有误
100030003       |登录发生数据库错误
10004       |查询登录token相关错误码
100040001       |用户id不错在
100040002       |用户登录token不错在

## 整体安全设计
### 客户端服务端通讯安全
- 通讯层基于Https安全通讯机制
- 业务层服务端使用预埋的RSA公钥加密敏感数据后才发送给服务端

### 客户端安全
- 客户端启动时生成AES密钥，用于加密保存登录后的token

### 服务端安全
- 登录Token生成规则加入随机数和时间戳，使攻击者无法枚举所有Token
- SQL防注入
  - 客户端限制输入只允许数字加字母的组合，并限制长度
  - 服务端会检查所有的输入参数是否包含'=', '\', '/', '''等特殊字符，如果包含的话，则会返回错误
  - 所有传进来的参数都会通过mysqlpp转义，不会被mysql引擎执行
- 用户密码使用bcrypt hash，并加入每个用户都不同的随机盐，即使被拖库攻击者也无法一次性通过彩虹表猜出所有用户的弱密码，同时bcrypt也限制了攻击者遍历一次彩虹表需要几年的时间
- 服务端接收到敏感数据后，会通过RSA私钥解密，防止攻击者篡改通讯数据

## MySQL表设计
整体包含三张表
### 用户信息表 
表名：t_user
字段名称        |字段类型  |主键|字段说明
------------|-----------|-----------|-----------
user_name       |CHAR(20)        |Yes    |保存用户id
user_password       |CHAR(128)   |No     |保存加盐bycryt hash后的用户密码

### 用户登录token表
表名：t_user_token
字段名称        |字段类型  |主键|字段说明
------------|-----------|-----------|-----------
user_name       |CHAR(20)        |Yes    |保存用户id
user_token       |CHAR(128)   |No     |保存用户登录后的token

### 用户随机盐表
表名：t_user_random_hash
字段名称        |字段类型  |主键|字段说明
------------|-----------|-----------|-----------
user_name       |CHAR(20)        |Yes    |保存用户id
user_hash_salt       |CHAR(128)   |No     |保存用户的随机hash盐

## 登录用户Token生成规则
MD5(用户名 + 时间戳 + 随机数)

## 服务端日志文件解析
下面是服务端日志文件的一个具体实例：
> [2020-06-13 12:44:18][0d6b148c-b30a-4f2e-a6b0-08995a07e54c][DEBUG]Server init success
> 
> [2020-06-13 12:44:18][0d6b148c-b30a-4f2e-a6b0-08995a07e54c][DEBUG]Server listening on 0.0.0.0:50051
> 
> [2020-06-13 12:44:40][7962f001-1ed0-4c44-83a3-158e3cd348ab][DEBUG]Check SQL parameter str = 123456

服务端日志文件格式是这样的：
> [时间][SessionID][DEBUG或者ERROR] 日志内容
- [时间]包括具体的日期和时间
- [SessionID]每次新请求都会更新，可以用SessionID来区分不同的请求
- [DEBUG]表示是调试日性志，[ERROR]表示是错误日志
- 每一行的最后是具体的日志内容

## 使用到的工具的版本号
工具名字        |版本号       |组件说明
------------|-----------|-----------
Android Studio       |3.6.3        |编辑和编译Android工程
Android NDK       |r21        |Android native代码交叉编译工具链
bazel       |0.29.0        |服务端编译工具
djinni       |0.11.1b       |客户端Java代码和c++的jni层自动胶水代码实现
openssl       |1.1.1g       |客户端服务端非对称加密实现库
grpc       |1.29.1      |客户端服务端远程方法调用实现库
docker       |19.03.8      |容器化实现工具
mysql       |5.7.27      |服务端数据库实现
mysql++       |3.2.5      |mysql连接与查询的c++ sdk库

## 客户端编译运行步骤
将客户端工程导入对应版本的Android Studio既可编译运行

## 编译运行环境构建
编译运行环境都是Ubuntu环境，在Ubuntu容器的shell下，可以通过下面的命令构建完整的编译运行环境
```
# 安装依赖组件
sudo apt-get update && apt-get install -y sudo
sudo apt-get install -y build-essential git \
    mysql-server mysql-client cmake autoconf libmysqlclient-dev libmysqld-dev \
    libmysql++-dev libmysql++-doc libmysql++3v5 libtool pkg-config automake \
    curl make g++ unzip wget zip zlib1g-dev python3 python && sudo apt-get clean

# 安装bazel
RUN wget https://github.com/bazelbuild/bazel/releases/download/0.29.0/bazel-0.29.0-installer-linux-x86_64.sh && chmod +x bazel-0.29.0-installer-linux-x86_64.sh && \
    ./bazel-0.29.0-installer-linux-x86_64.sh --user && export PATH="$PATH:$HOME/bin"

# 编译安装bcrypt
git clone https://github.com/trusch/libbcrypt
cd libbcrypt
mkdir build && cd build && cmake .. && make && sudo make install && sudo ldconfig
```



## 服务端编译运行需要用到的命令
```
# 移动到源码所在目录
cd SingleLoginServer

# 启动mysql组件
sudo service mysql restart

# 编译生成服务端二进制文件并把生成的二进制文件从编译缓存里拷贝出来
export CC=gcc&&~/bin/bazel build //examples:greeter_server --sandbox_debug && rm ../greeter_server  && cp ./bazel-bin/examples/greeter_server ../greeter_server

# 启动服务端二进制文件
# 这里传入了两个参数给服务端二进制文件
# 第一个参数是指定的服务端配置文件路径
# 第二个参数是指定的服务端日志文件生成目录
../greeter_server /single_login_src/SingleLoginServer/etc/SingleLoginServerConfig.json /single_login_src/SingleLoginServer/etc


# 移动到你指定的日志生成目录，然后执行以下命令就能实时查看服务端生成的日志
# 日志文件日期要和当前日期匹配
tail -f log_2020-06-13.txt
```

## 服务端json配件解析
服务端的Json配置如下：
```
{
    "ServerCertPath": "/single_login_src/SingleLoginServer/etc/serverCert.cert",
    "ServerCertPrivateKeyPath": "/single_login_src/SingleLoginServer/etc/serverPrikey.key",
    "ServerDecryptPrivateKeyPath": "/single_login_src/SingleLoginServer/etc/serverDecryptPrivatekey.key",
    "ServerDatabaseName": "single_login",
    "ServerDatabaseIP": "localhost",
    "ServerDatabaseUserName": "root",
    "ServerDatabaseUserPassword": "12345678",
    "ServerIPAndPort": "0.0.0.0:50051"
 }
```
各字段的含义如下：
字段名        |字段类型       |字段说明
------------|-----------|-----------
ServerCertPath       |string        |服务端配置的Https证书路径
ServerCertPrivateKeyPath       |string        |服务端配置的Https证书对应的私钥路径
ServerDecryptPrivateKeyPath       |string       |服务端解密客户端发送过来的内容用到的RSA私钥路径
ServerDatabaseName       |string       |服务端连接MySQL数据库的数据库名
ServerDatabaseIP       |string      |服务端连接MySQL数据库的数据库ip地址
ServerDatabaseUserName       |string      |服务端连接MySQL数据库的数据库用户名
ServerDatabaseUserPassword       |string      |服务端连接MySQL数据库的数据库用户密码
ServerIPAndPort       |string      |服务端监听的ip和端口




## 代码结构
### 客户端代码
- 客户端的java层核心代码在clientCode/AndroidLoginClient/app/src/main/java文件夹里面
- 客户端的native层核心代码基本都在SingleLoginServer/Client/ClientSource/src/java/com/faip/piggy文件夹里

### 服务端代码
- 服务端的核心代码都在SingleLoginServer/Server/ServerSource/src文件夹下
- 核心文件是greeter_server.cc

## 遗留问题与未来可优化方向
由于时间关系，本项目目前还是有一些地方设计的并不是那么合理，如果有更多时间的话可以针对性的优化以下一些问题：
- 客户端引入弱密码库，阻止用户用弱账号弱密码组合来注册
- 调通grpc流式连接，做到能实时推送消息给客户端，而不是现在客户端轮训消息
- 引入服务端Redis组件来保存登录后的用户token，存取都更加高效
- 将mysql数据库和redis组件都单独部署到一个容器上，防止各组件的独立性避免互相影响性能和稳定性
- 日志文件使用mmap内存映射，写日志的效率会更高




