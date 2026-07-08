#include "clientclass.h"

ClientClass::ClientClass(IKernel* pKernel) {
    m_bQuitFlag = true;
    m_hThread = 0;
    m_pKernel = pKernel;
}

ClientClass::~ClientClass()
{
    closesocket(sockclient);
    WSACleanup();
}

bool ClientClass::ConnectServer(const char *szip, short nport)
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
        printf("Client The Winsock 2.2 dll was found okay\n");
    //2、创建套接字
    sockclient = socket(AF_INET,SOCK_STREAM,0);
    if(sockclient == INVALID_SOCKET){
        cout<<"socket error"<<endl;
        closesocket(sockclient);
        WSACleanup();
        return false;
    }
    //3、连接服务器
    sockaddr_in addrserver;
    addrserver.sin_addr.S_un.S_addr = inet_addr(szip);
    addrserver.sin_family = AF_INET;
    addrserver.sin_port = htons(nport);
    if(connect(sockclient,(sockaddr*)&addrserver,sizeof(addrserver)) == SOCKET_ERROR){
        cout<<"connect error"<<endl;
        closesocket(sockclient);
        WSACleanup();
        return false;
    }
    //为recv创建线程
    m_hThread = CreateThread(0,0,&ThreadRecv,this,0,0);
    return true;
}

void ClientClass::disConnectServer()
{
    m_bQuitFlag = false;
    if(m_bQuitFlag){
        if(WaitForSingleObject(m_hThread,100)==WAIT_TIMEOUT){
            TerminateThread(m_hThread,-1);
            CloseHandle(m_hThread);
            m_hThread=0;
        }
    }
}

bool ClientClass::SendData(const char *szbuf, int nLen)
{
    //校验参数
    if(!szbuf||nLen<=0||sockclient==INVALID_SOCKET){
        return false;
    }
    //包大小
    if(send(sockclient,(char*)&nLen,sizeof(int),0)<=0){
        return false;
    }
    //包内容
    if(send(sockclient,szbuf,nLen,0)<=0)
        return false;
    return true;
}

void ClientClass::RecvData()
{
    //先接受包大小
    int nPackSize;
    int nRecvNum=recv(sockclient,(char*)&nPackSize,sizeof(int),0);
    if(nRecvNum<=0){
        closesocket(sockclient);
        return;
    }
    //再接收包内容
    char* pszbuf = new char[nPackSize];
    int noffset = 0;
    while(nPackSize){
        nRecvNum = recv(sockclient,pszbuf+noffset,nPackSize,0);
        noffset+=nRecvNum;
        nPackSize-=nRecvNum;
        //cout<<nPackSize<<" ";
    }
    //cout<<pszbuf<<endl;
    m_pKernel->DealData(pszbuf);
    Sleep(100);
    delete[] pszbuf;
    pszbuf = nullptr;


    return;
}

DWORD ClientClass::ThreadRecv(LPVOID lp)
{
    ClientClass* pThis=(ClientClass*)lp;
    while(pThis->m_bQuitFlag){
        pThis->RecvData();
    }
    return 0;
}
