#ifndef MYSQLPP_QUERY_UTILS_INCLUDED
#define MYSQLPP_QUERY_UTILS_INCLUDED

#include <iostream>
#include <stdarg.h>
#include <mysql++/mysql++.h>

using namespace std;
using namespace mysqlpp;

typedef struct MySQLQueryResult {
    int queryResultRowCount = -1;
    std::string error = "";
    bool querySuccess = false;
    StoreQueryResult ares;
} MySQLQueryResult;

MySQLQueryResult mySQLQuery(const char* dbName, 
                            const char* dbIP, 
                            const char* dbUserName, 
                            const char* dbUserPwd, 
                            const string queryStr,
                            const string queryParam1,
                            const string queryParam2,
                            const string queryParam3);


#endif // MYSQLPP_QUERY_UTILS_INCLUDED