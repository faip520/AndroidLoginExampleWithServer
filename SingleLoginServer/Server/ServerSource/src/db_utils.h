#ifndef DB_UTILS_INCLUDED
#define DB_UTILS_INCLUDED

#include <memory>
#include <string>
#include <stdexcept>

using namespace std;

// db工具函数，判断用户名和对应的token是否在数据库中存在
bool isUserTokenExistInDB(string single_login_server_database_name,
                          string single_login_server_database_ip,
                          string single_login_server_database_username,
                          string single_login_server_database_password,
                          string user_name, 
                          string user_token);

// db工具函数，用户登录成功后，将用户对应的用户名和token保存起来
int saveUserTokenToDB(string single_login_server_database_name,
                      string single_login_server_database_ip,
                      string single_login_server_database_username,
                      string single_login_server_database_password,
                      string user_name,
                      string user_token);

// db工具函数，判断用户名是否已经在db中存在了
bool isUserExistInDB(string single_login_server_database_name,
                     string single_login_server_database_ip,
                     string single_login_server_database_username,
                     string single_login_server_database_password,
                     string user_name);

// db工具函数，判断用户名和bcrypt加密过后的密码是否已经在数据库中存在，存在则认为登录成功
bool isUserAndPasswordExistInDB(string single_login_server_database_name,
                                string single_login_server_database_ip,
                                string single_login_server_database_username,
                                string single_login_server_database_password,
                                string user_name, 
                                string password);

// db工具函数，将用户名和用户对应的随机hash盐保存起来
bool saveUserHashSaltToDB(string single_login_server_database_name,
                          string single_login_server_database_ip,
                          string single_login_server_database_username,
                          string single_login_server_database_password,
                          string user_name, 
                          string& hashSalt);

// db工具函数，获取保存在数据库的用户随机hash盐
string getUserHashSalt(string single_login_server_database_name,
                       string single_login_server_database_ip,
                       string single_login_server_database_username,
                       string single_login_server_database_password,
                       string user_name);

// db工具函数，注册时，将用户名和密码保存起来，保存的是bcrypt加随机盐过后的密码
int saveUserInfoToDB(string single_login_server_database_name,
                     string single_login_server_database_ip,
                     string single_login_server_database_username,
                     string single_login_server_database_password,
                     string user_name, 
                     string user_password);

#endif // DB_UTILS_INCLUDED