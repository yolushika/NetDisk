#ifndef PACKDEF_H
#define PACKDEF_H

#define _default_protocol_base 0
//注册
#define _default_protocol_register_rq _default_protocol_base+1
#define _default_protocol_register_rs _default_protocol_base+2
//登录
#define _default_protocol_login_rq _default_protocol_base+3
#define _default_protocol_login_rs _default_protocol_base+4
//获取文件列表
#define _default_protocol_getfilelist_rq _default_protocol_base+5
#define _default_protocol_getfilelist_rs _default_protocol_base+6
//上传文件信息
#define _default_protocol_uploadfileinfo_rq _default_protocol_base+7
#define _default_protocol_uploadfileinfo_rs _default_protocol_base+8
//上传文件内容
#define _default_protocol_uploadfilecontent_rq _default_protocol_base+9
#define _default_protocol_uploadfilecontent_rs _default_protocol_base+10
//删除文件
#define _default_protocol_deleteflie_rq _default_protocol_base+11
#define _default_protocol_deleteflie_rs _default_protocol_base+12
//下载文件信息
#define _default_protocol_downloadfileinfo_rq _default_protocol_base+13
#define _default_protocol_downloadfileinfo_rs _default_protocol_base+14
//下载文件内容
#define _default_protocol_downloadfilecontent_rq _default_protocol_base+15
#define _default_protocol_downloadfilecontent_rs _default_protocol_base+16
//查询
#define _default_protocol_select_rq _default_protocol_base+17
#define _default_protocol_select_rs _default_protocol_base+18
//分享
#define _default_protocol_sharelink_rq _default_protocol_base+19
#define _default_protocol_sharelink_rs _default_protocol_base+20
//提取
#define _default_protocol_getlink_rq _default_protocol_base+21
#define _default_protocol_getlink_rs _default_protocol_base+22

//注册结果
#define _register_res_failed 0
#define _register_res_success 1
//登录结果
#define _login_res_failde 0
#define _login_res_noexist 1
#define _login_res_success 2

//传输结果
#define _uploadfileinfo_repeat 0//重复上传
#define _uploadfileinfo_contuine 1//断点续传
#define _uploadfileinfo_flashtrans 2//秒传
#define _uploadfileinfo_normal 3//正常传

//提取链接
#define _getlink_failed 0
#define _getlink_success 1
//协议包
struct STRU_BASE{
    char m_nType;
};
#define MAX_SIZE 45
#define FILE_NUM 15
#define SQLLEN 300
#define FILE_PATH 260
#define ONE_PAGE 4096

//注册
struct STRU_REGISTER_RQ: public STRU_BASE{
    STRU_REGISTER_RQ(){
        m_nType = _default_protocol_register_rq;
    }
    char szName[MAX_SIZE];
    char szPassword[MAX_SIZE];
    long long szTel;
};
struct STRU_REGISTER_RS: public STRU_BASE{
    STRU_REGISTER_RS(){
        m_nType = _default_protocol_register_rs;
    }
    char szResult;
};
//登录
struct STRU_LOGIN_RQ: public STRU_BASE{
    STRU_LOGIN_RQ(){
        m_nType = _default_protocol_login_rq;
    }
    char szName[MAX_SIZE];
    char szPassword[MAX_SIZE];
};
struct STRU_LOGIN_RS: public STRU_BASE{
    STRU_LOGIN_RS(){
        m_nType = _default_protocol_login_rs;
    }
    char szResult;
    long long szUserId;//返还给客户端的ID，便于客户端查看文件列表
};
//注册
struct STRU_GETFILELIST_RQ: public STRU_BASE{
    STRU_GETFILELIST_RQ(){
        m_nType = _default_protocol_getfilelist_rq;
    }
    long long szUserId;
};
//查到是不是文件列表的信息(链表)
struct FileInfo{
    //文件名   文件上传时间  上传大小
    char szFileName[MAX_SIZE];
    char szFileUploadTime[MAX_SIZE];
    long long szFileSize;
};
struct STRU_GETFILELIST_RS: public STRU_BASE{
    STRU_GETFILELIST_RS(){
        m_nType = _default_protocol_getfilelist_rs;
    }
    FileInfo arrFileInfo[FILE_NUM];
    long szFileNum;//此数据包内真正的文件个数
};
struct STRU_UPLOADFILEINFO_RQ:public STRU_BASE{
    STRU_UPLOADFILEINFO_RQ(){
        m_nType = _default_protocol_uploadfileinfo_rq;
    }
    long long UserId;
    char szFileName[MAX_SIZE];
    char szFileUploadTime[MAX_SIZE];
    long long szfileSize;
    char szFileMD5[MAX_SIZE];
};

struct STRU_UPLOADFILEINFO_RS:public STRU_BASE{
    STRU_UPLOADFILEINFO_RS(){
        m_nType = _default_protocol_uploadfileinfo_rs;
    }
    char szFileMD5[MAX_SIZE];
    long long fileId;//文件的ID
    long long m_Pos;//文件的传输位置，传了一半，停止了
    char m_Result;//传输结果
};

struct STRU_UPLOADFILECONTENT_RQ:public STRU_BASE{
    STRU_UPLOADFILECONTENT_RQ(){
        m_nType = _default_protocol_uploadfilecontent_rq;
    }
    long long UserId;
    long long FileId;           //文件的id
    char m_FIleContent[ONE_PAGE];         //文件内容数组
    long m_FileNum;        //真正上传文件的大小
};
struct STRU_SELECTFILE_RQ:public STRU_BASE{
    STRU_SELECTFILE_RQ(){
        m_nType = _default_protocol_select_rq;
    }
    long long UserId;
    char m_KeyWord[MAX_SIZE];
};
struct STRU_SELECTFILE_RS: public STRU_BASE{
    STRU_SELECTFILE_RS(){
        m_nType = _default_protocol_select_rs;
    }
    FileInfo arrFileInfo[FILE_NUM];
    long szFileNum;//此数据包内真正的文件个数
};
struct STRU_DELETEFILE_RQ: public STRU_BASE{
    STRU_DELETEFILE_RQ(){
        m_nType = _default_protocol_deleteflie_rq;
    }
    long long userid;
    char szFileName[MAX_SIZE];
};
struct STRU_SHARELINK_RQ: public STRU_BASE{
    STRU_SHARELINK_RQ(){
        m_nType = _default_protocol_sharelink_rq;
    }
    long long userid;
    char szFileName[MAX_SIZE];
};
struct STRU_SHARELINK_RS: public STRU_BASE{
    STRU_SHARELINK_RS(){
        m_nType = _default_protocol_sharelink_rs;
    }
    char szCode[MAX_SIZE];
    char szFileName[MAX_SIZE];
};
struct STRU_GETLINK_RQ: public STRU_BASE{
    STRU_GETLINK_RQ(){
        m_nType = _default_protocol_getlink_rq;
    }
    long long userId;
    char szFileUploadTime[MAX_SIZE];
    char szCode[MAX_SIZE];
};
struct STRU_GETLINK_RS: public STRU_BASE{
    STRU_GETLINK_RS(){
        m_nType = _default_protocol_getlink_rs;
    }
    char szFileUploadTime[MAX_SIZE];
    char szFileName[MAX_SIZE];
    long long szFileSize;
    char szResult;
};
struct STRU_DOWNLOADFILE_RQ:public STRU_BASE{
    STRU_DOWNLOADFILE_RQ() {
        m_nType = _default_protocol_downloadfileinfo_rq;
    }
    long long userId;
    char szFileName[MAX_SIZE];
};
struct STRU_DOWNLOADFILE_RS:public STRU_BASE{
    STRU_DOWNLOADFILE_RS() {
        m_nType = _default_protocol_downloadfilecontent_rs;
    }
    char m_FileContent[ONE_PAGE];//文件内容数组
    long m_fileNum;//真正的传输的内容大小
};



#endif // PACKDEF_H
