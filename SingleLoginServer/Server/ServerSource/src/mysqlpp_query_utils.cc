#include <iostream>
#include <string>

#include "mysqlpp_query_utils.h"
#include "common_utils.h"
#include "log_file_utils.h"
#include "../../../mysqlpp/include/mysql++/result.h"

using namespace mysqlpp;
using namespace std;

//db工具函数，执行指定的mysql语句
MySQLQueryResult mySQLQuery(const char* dbName, 
                            const char* dbIP, 
                            const char* dbUserName, 
                            const char* dbUserPwd, 
                            const string queryStr,
                            const string queryParam1,
                            const string queryParam2,
                            const string queryParam3) {
    
    MySQLQueryResult result;

    loggerD("Do MySQL query function enter, param dbName = " + string(dbName));
    loggerD("Do MySQL query function enter, param dbIP = " + string(dbIP));
    loggerD("Do MySQL query function enter, param dbUserName = " + string(dbUserName));
    loggerD("Do MySQL query function enter, param dbUserPwd = " + string(dbUserPwd));
    loggerD("Do MySQL query function enter, param queryStr = " + queryStr);
    loggerD("Do MySQL query function enter, param queryParam1 = " + queryParam1);
    loggerD("Do MySQL query function enter, param queryParam2 = " + queryParam2);
    loggerD("Do MySQL query function enter, param queryParam3 = " + queryParam3);

    // 必要的参数检查
    if (isCharPointerEmpty(dbName)) {
        loggerE("Do MySQL query, param db name empty!");
        return result;
    }
    if (isCharPointerEmpty(dbIP)) {
        loggerE("Do MySQL query, param db ip empty!");
        return result;
    }
    if (isCharPointerEmpty(dbUserName)) {
        loggerE("Do MySQL query, param db user name empty!");
        return result;
    }
    if (isCharPointerEmpty(dbUserPwd)) {
        loggerE("Do MySQL query, param db user password empty!");
        return result;
    }
    if (queryStr.empty()) {
        loggerE("Do MySQL query, query string empty!");
        return result;
    }
    if (queryParam1.empty()) {
        loggerE("Do MySQL query, query param empty!");
        return result;
    }

    try {
        // 通过mysql++的接口建立数据库连接
        Connection conn(false);
        conn.connect(dbName, dbIP, dbUserName, dbUserPwd);
        Query query = conn.query();

        // 对mysql中的用户输入内容转义
        string* escaped_query_param = new string();
        query.escape_string(escaped_query_param, queryParam1.c_str());
        loggerD("Do MySQL query, query param1 escaped = " + (*escaped_query_param));

        string queryString = "";

        if (!queryParam2.empty() && !queryParam3.empty()) {
            // 对mysql中的用户输入内容转义
            string* escaped_query_param2 = new string();
            query.escape_string(escaped_query_param2, queryParam2.c_str());
            loggerD("Do MySQL query, query param2 escaped = " + (*escaped_query_param2));

            // 对mysql中的用户输入内容转义
            string* escaped_query_param3 = new string();
            query.escape_string(escaped_query_param3, queryParam3.c_str());
            loggerD("Do MySQL query, query param3 escaped = " + (*escaped_query_param3));
            
            queryString = formatStringThree(queryStr, *escaped_query_param, *escaped_query_param2, *escaped_query_param3);

            // 及时删除不再需要的堆内存对象
            delete escaped_query_param2;
            delete escaped_query_param3;
            
        } else if (!queryParam2.empty() ){
            // 对mysql中的用户输入内容转义
            string* escaped_query_param2 = new string();
            query.escape_string(escaped_query_param2, queryParam2.c_str());
            loggerD("Do MySQL query, query param2 escaped = " + (*escaped_query_param2));

            queryString = formatStringTwo(queryStr, *escaped_query_param, *escaped_query_param2);
            // 及时删除不再需要的堆内存对象
            delete escaped_query_param2;
        } else {
            queryString = formatStringOne(queryStr, *escaped_query_param);
        }
        // 及时删除不再需要的堆内存对象
        delete escaped_query_param;

        loggerD("Do MySQL query, query string with escaped param = " + queryString);

        // 输入完整的mysql语句
        query << queryString;
        
        // 执行mysql语句并返回结果
        StoreQueryResult ares = query.store();
        result.queryResultRowCount = ares.num_rows();
        result.ares = ares;
        loggerD("Do MySQL query, query result row count = " + result.queryResultRowCount);
        result.error = string(query.error());
        loggerD("Do MySQL query, query result error msg = " + result.error);
        if (result.error.empty()) {
            loggerD("Do MySQL query, query succeed!");
            result.querySuccess = true;
        } else {
            loggerE("Do MySQL query, query failed!");
        }

        return result;
    } catch (const BadQuery er) { 
        // 处理异常情况
        loggerE("Do MySQL query, bad query, what = " + string(er.what()));
        return result;
    } catch (const BadConversion& er) {
        // 处理异常情况
        loggerE("Do MySQL query, bad conversion, what = " + string(er.what()));
        return result;
    } catch (const Exception& er) {
        // 处理异常情况
        loggerE("Do MySQL query, exception, what = " + string(er.what()));
        return result;
    }
}