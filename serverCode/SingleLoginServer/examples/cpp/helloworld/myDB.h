//
// Created by willweizhao(赵伟) on 2019-09-09.
//

#ifndef SINGLELOGINSERVER_MYDB_H
#define SINGLELOGINSERVER_MYDB_H


#include<iostream>
#include<string>
#include "myDB.h"

using namespace std;

class myDB {
public:
    myDB();

    ~myDB();

    bool initDB(string host, string user, string pwd, string db_name);

    bool exeSQL(string sql);

private:
    //MYSQL *connection;
    //MYSQL_RES *result;
    //MYSQL_ROW row;
};


#endif //SINGLELOGINSERVER_MYDB_H
