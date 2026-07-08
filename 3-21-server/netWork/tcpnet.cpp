#include "tcpnet.h"
#include <winsock2.h>

TCPclass::TCPclass() {
    m_socketlisten=0;
    m_QuitFlag=true;
    FD_ZERO(&m_fdsets);
}

TCPclass::~TCPclass()
{
    closesocket(m_socketlisten);
    WSACleanup();
}

bool TCPclass::InitNetWork(unsigned long dwip, short nport)
{
    //1、加载库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return false;
    }
    else
        printf("server The Winsock 2.2 dll was found okay\n");
    //2、创建套接字
    m_socketlisten = socket(AF_INET,SOCK_STREAM,0);
    if(m_socketlisten == INVALID_SOCKET){
        cout<<"socket error"<<endl;
        closesocket(m_socketlisten);
        WSACleanup();
        return false;
    }
    //3、绑定端口号
    sockaddr_in addrServer;
    addrServer.sin_family = AF_INET;//地址簇
    addrServer.sin_port = htons(nport);//返回值是一个端口号
    addrServer.sin_addr.S_un.S_addr = dwip;//ip地址
    if(bind(m_socketlisten,(sockaddr*)&addrServer,sizeof(addrServer)) == SOCKET_ERROR){
        printf("bind error");
        closesocket(m_socketlisten);
        WSACleanup();
        return false;
    }
    //4、监听
    if(listen(m_socketlisten,128) == SOCKET_ERROR){//内存中一直监听 非阻塞
        cout<<"listen error";
        closesocket(m_socketlisten);
        WSACleanup();
        return false;
    }
    printf("listen seccuss %d！\n", nport);
    HANDLE hThread = CreateThread(0,0,&ThreadSelect,this,0,0);//安全属性 初始堆栈大小 线程函数地址 传给线程函数的值 标志
    if(hThread){
        m_lstHandle.push_back(hThread);
        cout<<"thread create success"<<endl;
    }
    // hThread = CreateThread(0,0,&ThreadRecv,this,0,0);
    // if(hThread){
    //     m_lstHandle.push_back(hThread);
    // }
    FD_SET(m_socketlisten,&m_fdsets);
    return true;
}

void TCPclass::UnitNetWork()
{
    m_QuitFlag = false;
    auto ite = m_lstHandle.begin();
    while(ite!=m_lstHandle.end()){
        if(WAIT_TIMEOUT == WaitForSingleObject(*ite,100)){
            TerminateThread(*ite,-1);
        }
        CloseHandle(*ite);
        ite++;
    }
    m_lstHandle.clear();
}

bool TCPclass::SendData(SOCKET sockwaitor, char* szbuf,int nLen)
{
    //校验参数
    if(!szbuf||nLen<=0||sockwaitor==INVALID_SOCKET){
        return false;
    }
    //包大小
    if(send(sockwaitor,(char*)&nLen,sizeof(int),0)<=0){
        return false;
    }
    //包内容
    if(send(sockwaitor,szbuf,nLen,0)<=0)
        return false;
    return true;
}

DWORD TCPclass::ThreadSelect(LPVOID lp)
{
    TCPclass* pthis = (TCPclass*)lp;
    TIMEVAL tv;
    tv.tv_sec=0;
    tv.tv_usec=100;
    sockaddr_in addrclient;
    int nLen,Num;
    int nReadNum,nPackSize;
    char* pszbuf;
    while(pthis->m_QuitFlag){
        //判断哪个套接字在select返回的集合中
        //定义临时集合
        fd_set fdtemp;
        fdtemp=pthis->m_fdsets;
        //每次在内存中查看fdtemp集合 其中哪些套接字发生了可读性的网络事件 一次查看tv(100)微
        //返回值是有几个套接字发生了网络事件
        Num = select(fdtemp.fd_count,&fdtemp,0,0,&tv);
        while(Num>0){//每循环一次接受一个网络事件
            if(FD_ISSET(pthis->m_socketlisten,&fdtemp)){
                    //有客户端来链接
                    nLen=sizeof(addrclient);
                SOCKET sockwaitor = accept(pthis->m_socketlisten,(sockaddr*)&addrclient,&nLen);//阻塞函数
                if(sockwaitor == INVALID_SOCKET){
                    continue;
                }
                cout<<"Client ip:"<<inet_ntoa(addrclient.sin_addr)<<"port:"<<addrclient.sin_port<<endl;
                //waitor代表着一个客户端
                pthis->m_lstSocket.push_back(sockwaitor);
                FD_SET(sockwaitor,&pthis->m_fdsets);
                FD_CLR(pthis->m_socketlisten,&fdtemp);
                Num--;
            }else{
                //判断waitor在不在集合中  可以遍历waitor链表 一一比较每一个waitor
                //证明客户端来消息了
                for(auto ite=pthis->m_lstSocket.begin();ite!=pthis->m_lstSocket.end();ite++){
                    SOCKET sockWaitor=*ite;
                    nReadNum = recv(sockWaitor,(char*)&nPackSize,sizeof(int),0);
                    if(nReadNum<=0){
                        Num--;
                        if(WSAGetLastError()==10054){
                            //客户端已下线
                            closesocket(sockWaitor);
                            FD_CLR(*ite,&pthis->m_fdsets);
                            ite=pthis->m_lstSocket.erase(ite);
                        }
                        continue;
                    }
                    int noffset=0;
                    pszbuf=new char[nPackSize];
                    while(nPackSize>0){
                        nReadNum=recv(sockWaitor,pszbuf+noffset,nPackSize,0);
                        noffset+=nReadNum;
                        nPackSize-=nReadNum;
                    }
                    //cout<<"Client say:"<<pszbuf<<endl;
                    Kernel::GetKernel()->dealData(*ite,pszbuf);
                    Sleep(100);
                    delete[] pszbuf;
                    pszbuf=nullptr;
                    Num--;
                }
            }
        }
    }
    return 0;
}

void TCPclass::recvdata()
{
    for(auto ite = m_lstSocket.begin();ite!=m_lstSocket.end();ite++)
    {
        SOCKET sockWaitor = *ite;
        //先接受包大小
        int nPackSize;
        int nRecvNum=recv(sockWaitor,(char*)&nPackSize,sizeof(int),0);
        if(nRecvNum<=0){//可能是客户端没有消息产生  或客户端关闭链接
            if(GetLastError()==10054){
                closesocket(*ite);
                ite=m_lstSocket.erase(ite);
            }
            continue;
        }
        //再接收包内容
        char* pszbuf = new char[nPackSize];
        int noffset = 0;
        while(nPackSize){
            nRecvNum = recv(sockWaitor,pszbuf+noffset,nPackSize,0);
            noffset+=nRecvNum;
            nPackSize-=nRecvNum;
        }
        cout<<pszbuf<<endl;
        delete[] pszbuf;
        pszbuf = nullptr;
    }
        Sleep(10);
}




DWORD TCPclass::ThreadRecv(LPVOID lp)
{
    TCPclass* pthis = (TCPclass*)lp;
    //SOCKET sockWaitor = pthis->m_mapThreadToSocket[GetCurrentThreadId()];
    while(pthis->m_QuitFlag){
        //先接收包大小
        // int nPackSize;
        // int nRecvNum = recv(sockWaitor,(char*)&nPackSize,sizeof(int),0);
        // if(nRecvNum>0){
        //     char* pszbuf = new char[nPackSize];
        //     int noffset = 0;
        //     while(nPackSize){
        //         nRecvNum = recv(sockWaitor,pszbuf+noffset,nPackSize,0);
        //         noffset+=nRecvNum;
        //         nPackSize-=nRecvNum;
        //         cout<<nPackSize<<" ";
        //     }
        //     cout<<pszbuf<<endl;
        //     delete[] pszbuf;
        //     pszbuf = nullptr;
        // }
        pthis->recvdata();
        Sleep(10);
    }
    return 0;
}

