#ifndef CMYSQL_H
#define CMYSQL_H
#include <./mysql.h>
#include <QDebug>
#include <string>
#include <list>
using namespace std;

class CMysql
{
public:
    CMysql();
    ~CMysql();
public:
    bool ConnectMySql(const char* ip,const char* user,const char* password,const char* db);
    void DisConnect();
    bool SelectMysql(const char* sql,int nColumn,list<string>& lst); //执行select操作
    bool UpdateMysql(const char* sql); //执行insert update delete操作
private:
    MYSQL* mysql;
    MYSQL_RES* result;
    MYSQL_ROW row;
};
/*
    连接数据库
    拼写sql语句
    sql语句交给MYSQL执行
    处理结果 select     update delete insert
    关闭连接
    用的是MYSQL中的API
*/
#endif // CMYSQL_H
