#ifndef INET_H
#define INET_H
#include <winsock2.h>
class INet{
public:
    INet(){}
    virtual ~INet(){}
public:
    virtual bool ConnectServer(const char* szip="127.0.0.1",short nport = 8899)=0;
    virtual void disConnectServer()=0;
    virtual bool SendData(const char* szbuf,int nLen)=0;
    virtual void RecvData()=0;
};

#endif // INET_H
