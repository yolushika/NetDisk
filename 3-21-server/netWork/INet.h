#ifndef INET_H
#define INET_H
#include <winsock2.h>
class INet{
public:
    INet(){}
    virtual ~INet(){}
public:
    virtual bool InitNetWork(unsigned long dwip = 0,short nport = 8899) = 0;//默认参数 服务器ip和端口号
    virtual void UnitNetWork() = 0;
    virtual bool SendData(SOCKET sockwaitor, char* szbuf,int nLen) = 0;
    virtual void recvdata() = 0;
};

#endif // INET_H
