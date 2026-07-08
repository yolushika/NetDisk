#ifndef KERNEL_H
#define KERNEL_H
#pragma once
#include "IKernel.h"
#include "../TcpNet/clientclass.h"
#include "packdef.h"
#include <QObject>

class Kernel : public QObject, public IKernel
{
    Q_OBJECT
public:
    explicit Kernel(QObject *parent = nullptr);
    ~Kernel();
public:
    bool Connect(const char* szip="127.0.0.1",short nport = 8899);
    void DisConnect();
    bool SendData(const char* szbuf,int nLen);
    void DealData(const char* szbuf);
signals:
    void LoginRs(const char* szbuf);
    void RegisterRs(const char* szbuf);    
    void GetFileListRs(const char* szbuf);
    void UploadFileInfoRs(const char* szbuf);
    void Select(const char* szbuf);
    void SharedLink(const char* szbuf);
    void GetLink(const char* szbuf);
    void DownloadFile(const char* szbuf);
private:
    INet* m_pNet;
};

#endif // KERNEL_H
