#include "kernel.h"

Kernel::Kernel(QObject *parent)
    : QObject{parent}
{
    m_pNet = new ClientClass(this);
}

Kernel::~Kernel()
{
    disconnect();
    delete m_pNet;
    m_pNet = NULL;
}

bool Kernel::Connect(const char *szip, short nport)
{
    if(m_pNet->ConnectServer(szip,nport))
        return true;
    return false;
}

void Kernel::DisConnect()
{
    m_pNet->disConnectServer();
}

bool Kernel::SendData(const char *szbuf, int nLen)
{
    if(m_pNet->SendData(szbuf,nLen))
        return true;
    return false;
}

void Kernel::DealData(const char *szbuf)
{
    switch(*szbuf){
    case _default_protocol_login_rs:
        emit LoginRs(szbuf);
        break;
    case _default_protocol_register_rs:
        emit RegisterRs(szbuf);
        break;
    case _default_protocol_getfilelist_rs:
        emit GetFileListRs(szbuf);
        break;
    case _default_protocol_uploadfileinfo_rs:
        emit UploadFileInfoRs(szbuf);
        break;
    case _default_protocol_select_rs:
        emit Select(szbuf);
        break;
    case _default_protocol_sharelink_rs:
        emit SharedLink(szbuf);
        break;
    case _default_protocol_getlink_rs:
        emit GetLink(szbuf);
        break;
    case _default_protocol_downloadfilecontent_rs:
        emit DownloadFile(szbuf);
        break;
    }
}
