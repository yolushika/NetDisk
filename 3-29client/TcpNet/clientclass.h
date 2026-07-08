#ifndef CLIENTCLASS_H
#define CLIENTCLASS_H
#pragma once
#include <iostream>
#include "INet.h"
#include "../Kernel/kernel.h"

using namespace std;

class ClientClass:public INet
{
public:
    ClientClass(IKernel* pKernel);
    ~ClientClass();
public:
    bool ConnectServer(const char* szip="127.0.0.1",short nport = 8899);
    void disConnectServer();
    bool SendData(const char* szbuf,int nLen);
    void RecvData();
public:
    static DWORD WINAPI ThreadRecv(LPVOID lp);
private:
    SOCKET sockclient;
    bool m_bQuitFlag;
    HANDLE m_hThread;
    IKernel* m_pKernel;

};

#endif // CLIENTCLASS_H
