#ifndef IKERNEL_H
#define IKERNEL_H
class IKernel{
public:
    IKernel(){}
    virtual ~IKernel(){}
public:
    virtual bool Connect(const char* szip="127.0.0.1",short nport = 8899)=0;
    virtual void DisConnect()=0;
    virtual bool SendData(const char* szbuf,int nLen)=0;
    virtual void DealData(const char* szbuf)=0;
};

#endif // IKERNEL_H
