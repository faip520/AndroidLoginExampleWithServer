#include <iostream>
#include <bcrypt/BCrypt.hpp>
#include <md5Utils.h>
#include <piggy_base64.h>
#include <mysql++/mysql++.h>

#include "db_utils.h"
#include "common_utils.h"
#include "uuid_utils.h"
#include "error_code.h"
#include "mysqlpp_query_utils.h"
#include "log_file_utils.h"

using namespace std;

// db工具函数，判断用户名和对应的token是否在服务端存在
bool isUserTokenExistInDB(string single_login_server_database_name,
                          string single_login_server_database_ip,
                          string single_login_server_database_username,
                          string single_login_server_database_password,
                          string user_name, 
                          string user_token) {

    loggerD("Check user token exist in db, user name = " + user_name);
    loggerD("Check user token exist in db, user token = " + user_token);

    string queryString = "select * from t_user_token where user_name='%s' and user_token='%s'";

    loggerD("Check user token exist in db, query string = " + queryString);
    MySQLQueryResult queryResult = mySQLQuery(
                    single_login_server_database_name.c_str(), 
                    single_login_server_database_ip.c_str(), 
                    single_login_server_database_username.c_str(), 
                    single_login_server_database_password.c_str(),
                    queryString,
                    user_name,
                    user_token,
                    "");

    if (!queryResult.querySuccess) {
        return false;
    }

    return queryResult.queryResultRowCount == 1;
}

// db工具函数，用户登录成功后，将用户对应的用户名和token保存起来
int saveUserTokenToDB(string single_login_server_database_name,
                      string single_login_server_database_ip,
                      string single_login_server_database_username,
                      string single_login_server_database_password,
                      string user_name, 
                      string user_token) {
    loggerD("Save user token to db, user name = " + user_name);
    loggerD("Save user token to db, user token = " + user_token);

    string insertString = "insert into t_user_token values('%s', '%s') ON DUPLICATE KEY UPDATE user_token='%s'";

    loggerD("Save user token to db, insert string = " + insertString);
    MySQLQueryResult queryResult = mySQLQuery(
                    single_login_server_database_name.c_str(), 
                    single_login_server_database_ip.c_str(), 
                    single_login_server_database_username.c_str(), 
                    single_login_server_database_password.c_str(),
                    insertString,
                    user_name,
                    user_token,
                    user_token);

    if (!queryResult.querySuccess) {
        return ERR_CODE_SYSTEM_ERROR;
    } else {
        return ERR_CODE_OK;
    }
}

// db工具函数，判断用户名是否已经在db中存在了
bool isUserExistInDB(string single_login_server_database_name,
                     string single_login_server_database_ip,
                     string single_login_server_database_username,
                     string single_login_server_database_password,
                     string user_name) {
    string queryString = "select * from t_user where user_name='%s'";
    loggerD("Check user name exist in db, query string = " + queryString);

    MySQLQueryResult queryResult = mySQLQuery(
                    single_login_server_database_name.c_str(), 
                    single_login_server_database_ip.c_str(), 
                    single_login_server_database_username.c_str(), 
                    single_login_server_database_password.c_str(),
                    queryString,
                    user_name,
                    "",
                    "");

    loggerD("Check user name exist in db, query is succeed = " + queryResult.querySuccess);
    loggerD("Check user name exist in db, query result row count = " + queryResult.queryResultRowCount);
    loggerD("Check user name exist in db, query error msg = " + queryResult.error);

    if (!queryResult.querySuccess) {
        return true;
    }

    return queryResult.queryResultRowCount == 1;
}

// db工具函数，判断用户名和bcrypt加密过后的密码是否已经在数据库中存在，存在则认为登录成功
bool isUserAndPasswordExistInDB(string single_login_server_database_name,
                                string single_login_server_database_ip,
                                string single_login_server_database_username,
                                string single_login_server_database_password,
                                string user_name, 
                                string password) {
    string queryString = "select * from t_user where user_name='%s'";
    loggerD("Check user name and hashed password exist in db, query string = " + queryString);

    MySQLQueryResult queryResult = mySQLQuery(
                    single_login_server_database_name.c_str(), 
                    single_login_server_database_ip.c_str(), 
                    single_login_server_database_username.c_str(), 
                    single_login_server_database_password.c_str(),
                    queryString,
                    user_name,
                    "",
                    "");

    loggerD("Check user name and hashed password exist in db, query is succeed = " + queryResult.querySuccess);
    loggerD("Check user name and hashed password exist in db, query result row count = " + queryResult.queryResultRowCount);
    loggerD("Check user name and hashed password exist in db, query error msg = " + queryResult.error);

    if (!queryResult.querySuccess) {
        return false;
    }

    if (queryResult.queryResultRowCount != 1) {
        loggerE("Check user name and hashed password exist in db, query result more than one row = " + queryResult.queryResultRowCount);
        return false;
    }

    mysqlpp::String temp = queryResult.ares[0]["user_password"];
    string pwd_bcrypt = std::string(temp.data(),temp.length());
    loggerD("Check user name and hashed password exist in db, pwd_bcrypt = " + pwd_bcrypt);

    string userHashSalt = getUserHashSalt(single_login_server_database_name,
                                          single_login_server_database_ip,
                                          single_login_server_database_username,
                                          single_login_server_database_password,
                                          user_name);
    if (userHashSalt.empty()) {
        loggerE("Check user name and hashed password exist in db, get hash salt failed!");
        return false;
    }

    // 进行BCrypt校验
    BCrypt bcrypt;
    if (bcrypt.validatePassword(password + userHashSalt, pwd_bcrypt)) {
        loggerD("Check user name and hashed password exist in db, bcrypt check ok!");
        return true;
    } else {
        loggerE("Check user name and hashed password exist in db, bcrypt check failed!");
        return false;
    }
}

// db工具函数，将用户名和用户对应的随机hash盐保存起来
bool saveUserHashSaltToDB(string single_login_server_database_name,
                          string single_login_server_database_ip,
                          string single_login_server_database_username,
                          string single_login_server_database_password,
                          string user_name, 
                          string& hashSalt) {
    loggerD("Save use hash salt to db, user name = " + user_name);

    string randomSalt = generate_uuid_v4();
    loggerD("Save use hash salt to db, generated random salt = " + randomSalt);

    string insertString = "insert into t_user_random_hash values('%s', '%s')";
    loggerD("Save user has salt to db, insert string = " + insertString);

    MySQLQueryResult queryResult = mySQLQuery(
                    single_login_server_database_name.c_str(), 
                    single_login_server_database_ip.c_str(), 
                    single_login_server_database_username.c_str(), 
                    single_login_server_database_password.c_str(),
                    insertString,
                    user_name,
                    randomSalt,
                    "");

    loggerD("Save user hash salt to db, insert result is = " + queryResult.querySuccess);
    loggerD("Save user hash salt to db, insert result row count = " + queryResult.queryResultRowCount);
    loggerD("Save user hash salt to db, insert error msg = " + queryResult.error);

    if (!queryResult.querySuccess) {
        loggerE("Save user hash salt to db, insert failed!");
        return false;
    }

    if (queryResult.queryResultRowCount == 0) {
        loggerD("Save user hash salt to db, insert succeed!");
        // 将随机盐结果返回给外部
        hashSalt = randomSalt;
        return true;
    } else {
        loggerE("Save user hash salt to db, insert failed, row count not zero = " + queryResult.queryResultRowCount);
        return false;
    }
}

// db工具函数，获取保存在数据库的用户随机hash盐
string getUserHashSalt(string single_login_server_database_name,
                       string single_login_server_database_ip,
                       string single_login_server_database_username,
                       string single_login_server_database_password,
                       string user_name) {
    loggerD("Get use hash salt, user name = " + user_name);

    string queryString = "select * from t_user_random_hash where user_name='%s'";

    loggerD("Get user hash salt, query string = " + queryString);

    MySQLQueryResult queryResult = mySQLQuery(
                    single_login_server_database_name.c_str(), 
                    single_login_server_database_ip.c_str(), 
                    single_login_server_database_username.c_str(), 
                    single_login_server_database_password.c_str(),
                    queryString,
                    user_name,
                    "",
                    "");
    
    if (!queryResult.querySuccess) {
        loggerE("Get user hash salt, query failed!");
        return "";
    }

    if (queryResult.queryResultRowCount != 1) {
        loggerE("Get user hash salt, query result more than one row = " + queryResult.queryResultRowCount);
        return "";
    }

    mysqlpp::String temp = queryResult.ares[0]["user_hash_salt"];
    string user_hash_salt = std::string(temp.data(),temp.length());
    loggerD("Get user hash salt, final result = " + user_hash_salt);

    return user_hash_salt;
}

// db工具函数，注册时，将用户名和密码保存起来，保存的是bcrypt加随机盐过后的密码
int saveUserInfoToDB(string single_login_server_database_name,
                     string single_login_server_database_ip,
                     string single_login_server_database_username,
                     string single_login_server_database_password,
                     string user_name, 
                     string user_password) {
    loggerD("Save user info to db, user name = " + user_name);

    if (isUserExistInDB(single_login_server_database_name,
                        single_login_server_database_ip,
                        single_login_server_database_username,
                        single_login_server_database_password,
                        user_name)) {
        loggerD("Save user info to db, user alread in db!");
        return ERR_CODE_USER_EXIST;
    } else {
        loggerD("Save user info to db, user not in db!");
    }

    string hashSalt;
    bool saveSaltResult = saveUserHashSaltToDB(single_login_server_database_name,
                                               single_login_server_database_ip,
                                               single_login_server_database_username,
                                               single_login_server_database_password,
                                               user_name, 
                                               hashSalt);
    loggerD("Save user info to db, hash salt got = " + hashSalt);
    if (!saveSaltResult) {
        loggerE("Save user info to db, save salt failed!");
        return ERROR_CODE_REGISTER_SAVE_SALT_FAILED;
    }
    
    BCrypt bcrypt;
    // 加上随机盐后在bcrypt hash
    user_password = bcrypt.generateHash(user_password + hashSalt);

    loggerD("Save user info to db, user password bcrypt = " + user_password);

    string insertString = "insert into t_user values('%s', '%s')";
    loggerD("Save user info to db, insert string = " + insertString);

    MySQLQueryResult queryResult = mySQLQuery(
                    single_login_server_database_name.c_str(), 
                    single_login_server_database_ip.c_str(), 
                    single_login_server_database_username.c_str(), 
                    single_login_server_database_password.c_str(),
                    insertString,
                    user_name,
                    user_password,
                    "");

    loggerD("Save user info to db, insert result is succeed = " + queryResult.querySuccess);
    loggerD("Save user info to db, insert result row count = " + queryResult.queryResultRowCount);
    loggerD("Save user info to db, insert error msg = " + queryResult.error);

    if (!queryResult.querySuccess) {
        loggerE("Save user info to db, insert failed!");
        return ERR_CODE_SYSTEM_ERROR;
    } else {
        loggerD("Save user info to db, insert ok!");
        return ERR_CODE_OK;
    }
}
