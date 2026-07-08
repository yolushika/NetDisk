#include "cmysql.h"

CMysql::CMysql() {
    //初始化
    //mysql_init(MYSQL* mysql)
    mysql = new MYSQL;
    mysql_init(mysql);
    //解决中文乱码 设置字符集
    mysql_set_character_set(mysql,"utf8");
}

CMysql::~CMysql()
{
    delete mysql;
    mysql_free_result(result);
}

bool CMysql::ConnectMySql(const char* ip,const char* user,const char* password,const char* db)
{
    /*
        mysql_real_connect()
        参数
        MYSQL* mysql变量
        const char* host 服务器地址
        const char* user 用户
        const char* password 密码
        const char* db 数据库
        unsigned int port 端口号
        0
        NULL
        0
    */
    if(NULL == mysql_real_connect(mysql,ip,user,password,db,0,NULL,0))
        return false;
    return true;
}

void CMysql::DisConnect()
{
    mysql_close(mysql);
}

bool CMysql::SelectMysql(const char* sql,int nColumn,list<string>& lst)
{
    /*
        mysql_query()
        MYSQL* mysql 对象
        const char* query 需要执行的SQL语句
    */
    if(mysql_query(mysql,sql)){
        qDebug()<<mysql_error(mysql)<<"没查到结果";
        return false;
    }
    /*
        MYSQL_RES* mysql_store_result(MYSQL*)   获取查询到的结果 存储到一个MYSQL_RES 对象中
        返回值 取到的表格
        MYSQL_RES 对象就代表查到的表格
    */
    result = mysql_store_result(mysql);
    if(result==NULL){
        qDebug() << mysql_error(mysql) << "没查到结果";
        return false;
    }else{
        while((row = mysql_fetch_row(result))!=NULL){
            for(int i=0;i<nColumn;i++){
                if(row[i]==NULL)
                    row[i] = (char*)"null";
                lst.push_back(row[i]);
            }
        }
    }
    return true;
}

bool CMysql::UpdateMysql(const char* sql)
{
    if(sql == NULL)
        return false;
    if(mysql_query(mysql,sql)){
        qDebug()<<mysql_error(mysql);
        return false;
    }
    return true;
}
