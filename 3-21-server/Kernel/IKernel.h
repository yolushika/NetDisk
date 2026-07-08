#ifndef IKERNEL_H
#define IKERNEL_H
#include <winsock2.h>
class IKernel{
public:
    IKernel(){}
    virtual ~IKernel(){}
public:
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual void dealData(SOCKET scoketWaiter,const char* zsbuf) = 0;//用来处理接收到的数据

};

#endif // IKERNEL_H
