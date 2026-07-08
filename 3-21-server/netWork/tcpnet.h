#ifndef TCPCLASS_H
#define TCPCLASS_H
#include <iostream>
#include "INet.h"
#pragma once
#include "../Kernel/kernel.h"
#include <map>
#include <list>
#include <cstdio>
using namespace std;

class TCPclass:public INet
{
public:
    TCPclass();
    ~TCPclass();
public:
    bool InitNetWork(unsigned long dwip = 0,short nport = 8899);//默认参数 服务器ip和端口号
    void UnitNetWork();
    bool SendData(SOCKET sockwaitor, char* szbuf,int nLen);
    void recvdata();
public:
    static DWORD WINAPI ThreadSelect(LPVOID lp);
    static DWORD WINAPI ThreadRecv(LPVOID lp);
private:
    SOCKET m_socketlisten;
    list<HANDLE> m_lstHandle;
    bool m_QuitFlag;
    map<DWORD,SOCKET> m_mapThreadToSocket;
    list<SOCKET> m_lstSocket;
    fd_set m_fdsets;
};

#endif // TCPCLASS_H
