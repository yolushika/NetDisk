#ifndef KERNEL_H
#define KERNEL_H
#include "IKernel.h"
#pragma once
#include "../netWork/tcpnet.h"
#include "../CMySQL/cmysql.h"
#include "../packdef.h"
#include <QDebug>
struct fileinfo{
    FILE* pFile;
    long long userId;
    long long fileId;
    long long FileSize;
    long long pos;
};

class Kernel:public IKernel
{
public:
    Kernel();
    Kernel(Kernel &pk);
    ~Kernel();
public:
    static Kernel* GetKernel();//单例模式
    bool open();
    void close();
    void dealData(SOCKET socketWaiter,const char* szbuf);//用来处理接收到的数据
    void RegisterRq(SOCKET socketWaiter,const char* szbuf);
    void LoginRq(SOCKET socketWaiter,const char* szbuf);
    void GetFileListRq(SOCKET socketWaiter,const char* szbuf);
    void UploadFileInfoRq(SOCKET socketWaiter,const char* szbuf);
    void UploadFileContentRq(SOCKET socketWaiter,const char* szbuf);
    void Select(SOCKET socketWaiter,const char* szbuf);
    void DeleteFile(SOCKET socketWaiter,const char* szbuf);
    void SharedLinkRq(SOCKET socketWaiter,const char* szbuf);
    void GetLinkRq(SOCKET socketWaiter,const char* szbuf);
    void DownloadFile(SOCKET socketWaiter,const char* szbuf);
public:
    static Kernel* m_pKernel;
    INet* m_pNet;
    CMysql* m_pSql;
    char m_szSystemPath[FILE_PATH];
    list<fileinfo*> m_lstFileInfo;
};

#endif // KERNEL_H
