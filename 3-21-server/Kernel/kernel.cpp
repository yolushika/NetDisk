#include "kernel.h"
Kernel* Kernel::m_pKernel = new Kernel;
//Kernel* Kernel::m_pKernel = nullptr;

Kernel::Kernel() {
    m_pNet = new TCPclass;
    m_pSql = new CMysql;
    strcpy(m_szSystemPath,"D:/Qt project/Netdisk-UserFiles/User`s Files/");
}

Kernel::~Kernel(){
    delete m_pNet;
    m_pNet = NULL;

    delete m_pSql;
    m_pSql = NULL;
}
//饿汉式
// Kernel *Kernel::GetKernel()//实现单例模式：饿汉式（支持线程安全）
// {
//     if(m_pKernel==nullptr){
//         m_pKernel = new Kernel;
//     }
//     return m_pKernel;
// }

Kernel *Kernel::GetKernel()//实现单例模式：饿汉式（支持线程安全）
{
    return m_pKernel;
}

bool Kernel::open()
{
    if(!m_pNet->InitNetWork()){
        qDebug()<<"net error";
        return false;
    }
    if(!m_pSql->ConnectMySql("127.0.0.1","root","123456","3-22NetDisk")){
        qDebug()<<"mysql error";
        return false;
    }
    return true;
}

void Kernel::close()
{
    m_pNet->UnitNetWork();
}

void Kernel::dealData(SOCKET socketWaiter, const char *szbuf)
{
    switch(*szbuf){
    case _default_protocol_register_rq:
        //客户端产生的是注册请求
        RegisterRq(socketWaiter,szbuf);
    break;
    case _default_protocol_login_rq:
        LoginRq(socketWaiter,szbuf);
        //客户端产生的是登录请求
        break;
    case _default_protocol_getfilelist_rq:
        GetFileListRq(socketWaiter,szbuf);
        break;
    case _default_protocol_uploadfileinfo_rq:
        UploadFileInfoRq(socketWaiter,szbuf);
        //获取文件信息请求
        break;
    case _default_protocol_uploadfilecontent_rq:
        UploadFileContentRq(socketWaiter,szbuf);
        //获取文件内容请求
        break;
    case _default_protocol_select_rq:
        Select(socketWaiter,szbuf);
        break;
        //删除文件请求
    case _default_protocol_deleteflie_rq:
        DeleteFile(socketWaiter,szbuf);
        break;
    case _default_protocol_sharelink_rq:
        SharedLinkRq(socketWaiter,szbuf);
        break;
    case _default_protocol_getlink_rq:
        GetLinkRq(socketWaiter,szbuf);
        break;
    case _default_protocol_downloadfileinfo_rq:
        DownloadFile(socketWaiter,szbuf);
        break;
    }
}

void Kernel::RegisterRq(SOCKET socketWaiter, const char *szbuf)
{
    //解析请求包中用户名与密码，手机号
    //写到数据库中
        //判断成功与失败 通过注册回复包，返还结果给客户端
    STRU_REGISTER_RQ* srr = (STRU_REGISTER_RQ*)szbuf;
    STRU_REGISTER_RS srs;
    char szsql[SQLLEN];
    char FilePath[FILE_PATH];
    list<string> lsStr;
    srs.szResult = _register_res_failed;
    sprintf(szsql,"insert into user(u_name,u_password,u_tel) values('%s','%s',%lld);",
            srr->szName,srr->szPassword,srr->szTel);
    if(m_pSql->UpdateMysql(szsql)){
        //成功 创建为该用户的文件夹 反馈注册结果
        srs.szResult = _register_res_success;
        sprintf(szsql,"select u_id from user where u_name = '%s'and u_password = '%s';",
                srr->szName,srr->szPassword);
        m_pSql->SelectMysql(szsql,1,lsStr);
        if(lsStr.size()>0){
            //为当前用户创建文件夹
            string UserId = lsStr.front();
            lsStr.pop_front();
            sprintf(FilePath,"%s%s",m_szSystemPath,UserId.c_str());
            CreateDirectoryA(FilePath,0);// 创建文件夹函数
        }
    }
    m_pNet->SendData(socketWaiter,(char*)&srs,sizeof(srs));
}

void Kernel::LoginRq(SOCKET socketWaiter, const char *szbuf)
{
    //根据用户名查询密码 三种情况
    STRU_LOGIN_RQ* slr = (STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS sls;
    sls.szResult = _login_res_noexist;
    char szsql[SQLLEN]={0};
    list<string> lsStr;
    sprintf(szsql,"select u_id, u_password from user where u_name = '%s';",slr->szName);
    m_pSql->SelectMysql(szsql,2,lsStr);
    if(lsStr.size()>0){
        //证明用户是存在的
        //密码比较
        string UserId = lsStr.front();
        lsStr.pop_front();

        string password = lsStr.front();
        lsStr.pop_front();

        sls.szResult = _login_res_failde;
        if(strcmp(password.c_str(),slr->szPassword)==0){
            //用户存在且密码正确
            sls.szResult = _login_res_success;
            sls.szUserId = _atoi64(UserId.c_str());
        }
    }
    //返还给客户端
    m_pNet->SendData(socketWaiter,(char*)&sls,sizeof(sls));

}

void Kernel::GetFileListRq(SOCKET socketWaiter, const char *szbuf)
{
    STRU_GETFILELIST_RQ* sgr = (STRU_GETFILELIST_RQ*)szbuf;
    STRU_GETFILELIST_RS sgs;
    char szsql[SQLLEN]={0};
    int index = 0;
    list<string> lsStr;
    //看是哪个人在获取文件信息
    sprintf(szsql,"select f_name,f_size,f_uploadtime from ufile where u_id=%lld;",sgr->szUserId);
    //数据查询此人上传过的文件
    m_pSql->SelectMysql(szsql,3,lsStr);
    //封装进GetFileListRs结构体
    while(lsStr.size()>0){
        string fileName = lsStr.front();
        lsStr.pop_front();

        string fileSize = lsStr.front();
        lsStr.pop_front();

        string fileTime = lsStr.front();
        lsStr.pop_front();
        //放入文件信息数组中的哪个
        strcpy(sgs.arrFileInfo[index].szFileName,fileName.c_str());
        strcpy(sgs.arrFileInfo[index].szFileUploadTime,fileTime.c_str());
        sgs.arrFileInfo[index].szFileSize = atoll(fileSize.c_str());
        index++;
        if(index == FILE_NUM || lsStr.size() == 0){
            sgs.szFileNum = index;
            //返还客户端
            m_pNet->SendData(socketWaiter,(char*)&sgs,sizeof(sgs));
            ZeroMemory(sgs.arrFileInfo,sizeof(sgs.arrFileInfo));//记忆清空
            index=0;
        }
    }

}

void Kernel::UploadFileInfoRq(SOCKET socketWaiter, const char *szbuf)
{
    //1、校验数据库是否存在文件
    STRU_UPLOADFILEINFO_RQ* sur = (STRU_UPLOADFILEINFO_RQ*)szbuf;
    char szsql[SQLLEN]={0};
    list<string>lsStr;
    STRU_UPLOADFILEINFO_RS sus;
    sus.m_Pos = 0;
    strcpy(sus.szFileMD5,sur->szFileMD5);
    sprintf(szsql,"select u_id,f_id from ufile where f_MD5 = '%s'",sur->szFileMD5);
    m_pSql->SelectMysql(szsql,2,lsStr);
    bool flag = false;
    auto ite = lsStr.begin();
    string FileId;
    while(ite!=lsStr.begin()){
        string UserId = *ite;
        long long Uid = atoll(UserId.c_str());
        ite++;
        FileId = *ite;
        ite++;
        if(Uid == sur->UserId){
            //（1）、自己传的，重复上传
            flag=true;
            sus.m_Result = _uploadfileinfo_repeat;
            sus.fileId = atoll(FileId.c_str());
            auto Fileite = m_lstFileInfo.begin();
            while(Fileite!=m_lstFileInfo.end()){
                if((*Fileite)->fileId==atoll(FileId.c_str())&&sur->UserId==(*Fileite)->userId){
                    //断点续传
                    sus.m_Pos = (*Fileite)->pos;
                    sus.m_Result = _uploadfileinfo_contuine;
                    break;
                }
                Fileite++;
            }
        }
    }
    if(lsStr.size()>0&&flag==false){
            sus.m_Result = _uploadfileinfo_flashtrans;
            //（2）、别人传的基引用计数实现的   秒传 计数+1
            sprintf(szsql,"update file set f_count=f_count+1 where f_id = %lld",atoll(FileId.c_str()));
            m_pSql->UpdateMysql(szsql);
            //修改映射
            sprintf(szsql,"insert into user_file(u_id,f_id,time) values(%lld,%lld,'%s')",
                    sur->UserId,atoll(FileId.c_str()),sur->szFileUploadTime);
            m_pSql->UpdateMysql(szsql);
        }
   if(lsStr.size()==0){
        //3、如果不存在 正常传输 记录文件信息到数据库，映射表做映射
        sus.m_Result = _uploadfileinfo_normal;
        char szFilePath[FILE_PATH]={0};
        sprintf(szFilePath,"%s%lld/%s",m_szSystemPath,sur->UserId,sur->szFileName);
        sprintf(szsql,"insert into file(f_name,f_uploadtime,f_path,f_md5,f_size)values('%s','%s','%s','%s','%lld')",
                        sur->szFileName,sur->szFileUploadTime,szFilePath,sur->szFileMD5,sur->szfileSize);
        m_pSql->UpdateMysql(szsql);
        //查找f_id
        sprintf(szsql,"select f_id from file where f_md5 = '%s'",sur->szFileMD5);
        m_pSql->SelectMysql(szsql,1,lsStr);
        if(lsStr.size()>0){
            string FileId = lsStr.front();
            sus.fileId = atoll(FileId.c_str());
            //修改映射
            sprintf(szsql,"insert into user_file(u_id,f_id,time)values(%lld,%lld,'%s')",
                    sur->UserId,atoll(FileId.c_str()),sur->szFileUploadTime);
            m_pSql->UpdateMysql(szsql);
        }
        FILE* pFile = fopen(szFilePath,"wb");//可写的二进制格式
        //记录文件指针 文件ID，fileid，userid，pos
        fileinfo *p = new fileinfo;
        p->userId = sur->UserId;
        p->fileId = sus.fileId;
        p->FileSize = sur->szfileSize;
        p->pos = 0;
        p->pFile = pFile;

        m_lstFileInfo.push_back(p);
    }
    //4发送回复
    m_pNet->SendData(socketWaiter,(char*)&sus,sizeof(sus));
}

void Kernel::UploadFileContentRq(SOCKET socketWaiter, const char *szbuf)
{
    //用来接受某个文件的内容
    STRU_UPLOADFILECONTENT_RQ* scr = (STRU_UPLOADFILECONTENT_RQ*)szbuf;
    fileinfo* p = NULL;
    auto ite = m_lstFileInfo.begin();
    while(ite!=m_lstFileInfo.end()){
        if((*ite)->fileId == scr->FileId&&(*ite)->userId == scr->UserId){
            //找到了当前文件内容的归属
            p = *ite;
            break;
        }
        ite++;
    }
    //根据文件指针写入内容
    size_t WriteNum = fwrite(scr->m_FIleContent,sizeof(char),scr->m_FileNum,p->pFile);
    if(WriteNum>0){
        p->pos += WriteNum;
        if(p->pos == p->FileSize){
            //当前文件内容书写完毕后
            fclose(p->pFile);
            delete p;
            m_lstFileInfo.erase(ite);
        }
    }
}

void Kernel::Select(SOCKET socketWaiter, const char *szbuf)
{
    STRU_SELECTFILE_RQ* ssr = (STRU_SELECTFILE_RQ*)szbuf;
    STRU_SELECTFILE_RS sss;
    char szsql[SQLLEN]={0};
    int index = 0;
    list<string> lsStr;
    //看是哪个人在获取文件信息
    sprintf(szsql,"select f_name,f_size,f_uploadtime from ufile where u_id=%lld and f_name like '%%%s%%';",ssr->UserId,ssr->m_KeyWord);
    //数据查询此人上传过的文件
    m_pSql->SelectMysql(szsql,3,lsStr);
    //封装进GetFileListRs结构体
    while(lsStr.size()>0){
        string fileName = lsStr.front();
        lsStr.pop_front();

        string fileSize = lsStr.front();
        lsStr.pop_front();

        string fileTime = lsStr.front();
        lsStr.pop_front();
        //放入文件信息数组中的哪个
        strcpy(sss.arrFileInfo[index].szFileName,fileName.c_str());
        strcpy(sss.arrFileInfo[index].szFileUploadTime,fileTime.c_str());
        sss.arrFileInfo[index].szFileSize = atoll(fileSize.c_str());
        index++;
        if(index == FILE_NUM || lsStr.size() == 0){
            sss.szFileNum = index;
            //返还客户端
            m_pNet->SendData(socketWaiter,(char*)&sss,sizeof(sss));
            ZeroMemory(sss.arrFileInfo,sizeof(sss.arrFileInfo));//记忆清空
            index=0;
        }
    }

}

void Kernel::SharedLinkRq(SOCKET socketWaiter, const char *szbuf)
{
    STRU_SHARELINK_RQ* pssr = (STRU_SHARELINK_RQ*)szbuf;
    STRU_SHARELINK_RS sss;
    char szsql[SQLLEN];
    list<string> lsStr;
    strcpy(sss.szFileName,pssr->szFileName);
    srand(time(NULL));
    //先生成四位随机码
    char szCode[MAX_SIZE];
    char c;
    for(int i=0;i<4;i++){
        int num = rand()%6;
        if(num<10)
            c = num + '0';
        else
            c = num - 10 + 'A';
        szCode[i] = c;
    }
    strcpy(sss.szCode,szCode);
    //user_share表中插入数据
    sprintf(szsql,"select f_id from ufile where u_id = %lld and f_name = '%s';",pssr->userid,pssr->szFileName);
    m_pSql->SelectMysql(szsql,1,lsStr);
    if(lsStr.size()>0){
        string fileId = lsStr.front();
        lsStr.pop_front();

        sprintf(szsql,"select count(*) from user_shared where uid = %lld and fid = %lld;",pssr->userid,atoll(fileId.c_str()));
        m_pSql->SelectMysql(szsql,1,lsStr);
        string number = lsStr.front();
        lsStr.pop_front();

        if(atoll(number.c_str()) == 0)
        {
            sprintf(szsql,"insert into user_shared(uid,fid,code) values(%lld,%lld,'%s');",pssr->userid,atoll(fileId.c_str()),szCode);
            m_pSql->UpdateMysql(szsql);
        }
        else{
            sprintf(szsql,"update user_shared set code = '%s' where uid = %lld and fid = %lld;",szCode,pssr->userid,atoll(fileId.c_str()));
            m_pSql->UpdateMysql(szsql);
        }
    }
    //返还给客户端一个回复
    m_pNet->SendData(socketWaiter,(char*)&sss,sizeof(sss));
}

void Kernel::GetLinkRq(SOCKET socketWaiter, const char *szbuf)
{
    STRU_GETLINK_RQ* psgr = (STRU_GETLINK_RQ*)szbuf;
    STRU_GETLINK_RS sgs;
    //code 码值查询数据
    char szsql[SQLLEN]={0};
    list<string> lsStr;
    sprintf(szsql,"select uid,fid from user_shared where code = '%s';",psgr->szCode);
    m_pSql->SelectMysql(szsql,2,lsStr);
    if(lsStr.size()>0){
        //判断uid
        string uid = lsStr.front();
        lsStr.pop_front();

        string fid = lsStr.front();
        lsStr.pop_front();

        if(atoll(uid.c_str()) == psgr->userId){
            sgs.szResult = _getlink_failed;
        }else{
            sgs.szResult = _getlink_success;
            //查询文件信息
            sprintf(szsql,"select f_name,f_size from ufile where f_id = %lld",atoll(fid.c_str()));
            m_pSql->SelectMysql(szsql,2,lsStr);
            if(lsStr.size()>0){
                strcpy(sgs.szFileName,lsStr.front().c_str());
                lsStr.pop_front();

                sgs.szFileSize = atoll(lsStr.front().c_str());
                lsStr.pop_front();

                strcpy(sgs.szFileUploadTime,psgr->szFileUploadTime);
            }
            sprintf(szsql,"insert into user_file(f_id,u_id,time) values(%lld,%lld,'%s');",atoll(fid.c_str()),psgr->userId,psgr->szFileUploadTime);
            if(m_pSql->UpdateMysql(szsql)){
                sprintf(szsql,"update file set f_count = f_count+1 where f_id = %lld;",atoll(fid.c_str()));
                m_pSql->UpdateMysql(szsql);
            }
        }
    }
    //返还给客户端
     m_pNet->SendData(socketWaiter,(char*)&sgs,sizeof(sgs));
}

void Kernel::DownloadFile(SOCKET socketWaiter, const char *szbuf)
{
    STRU_DOWNLOADFILE_RQ* sdq = (STRU_DOWNLOADFILE_RQ*)szbuf;
    STRU_DOWNLOADFILE_RS sds;
    char szsql[SQLLEN] = {0};
    list<string> lsStr;
    string fileName = sdq->szFileName;
    long long userId = sdq->userId;
    //根据sdq中的文件名和用户Id 去视图查询对应文件路径
    sprintf(szsql,"select f_path from ufile where u_id = %lld and f_name = '%s';",sdq->userId,sdq->szFileName);
    m_pSql->SelectMysql(szsql,1,lsStr);
    if(lsStr.size()>0){
        string filePath = lsStr.front();
        lsStr.pop_front();
        //找到后返回给客户端对应文件路径
        FILE* pfile = fopen(filePath.c_str(),"rb");
        int num;
        while((num = fread(sds.m_FileContent,1,ONE_PAGE,pfile))>0){
            sds.m_fileNum = num;
            m_pNet->SendData(socketWaiter,(char*)&sds,sizeof(sds));
        }
        fclose(pfile);
    }
}

void Kernel::DeleteFileW(SOCKET socketWaiter, const char *szbuf)
{
    //从数据库找到文件信息————文件引用计数  >1  修改引用计数值 =1 删除文件信息 并删除文件本身
    STRU_DELETEFILE_RQ* sdr = (STRU_DELETEFILE_RQ*)szbuf;
    char szsql[SQLLEN] = {0};
    list<string> lsStr;
    string fileName = sdr->szFileName;
    long long userId = sdr->userid;
    sprintf(szsql,"select f_id,f_count,f_path from ufile where f_name = '%s' and u_id = %lld",fileName.c_str(),userId);
    m_pSql->SelectMysql(szsql,3,lsStr);
    if(lsStr.size()>0){
        string f_id = lsStr.front();
        lsStr.pop_front();

        string f_count = lsStr.front();
        lsStr.pop_front();

        string f_path = lsStr.front();
        lsStr.pop_front();
        long long fcount = atoll(f_count.c_str());
        long long fid = atoll(f_id.c_str());
        //引用计数＞1 修改引用计数值
        if(fcount>1){
            //修改引用计数
            sprintf(szsql,"update file set f_count = f_count-1 where f_id = %lld",fid);
            m_pSql->UpdateMysql(szsql);
        }
        //引用计数=1 删除文件信息 并删除文件本身
        else{
            //删除文件信息
            sprintf(szsql,"delete from file where f_id = %lld",fid);
            m_pSql->UpdateMysql(szsql);
            //删除文件内容
            DeleteFileA(f_path.c_str());
        }
        //删除映射信息
        sprintf(szsql,"delete from user_file where u_id = %lld and f_id = %lld",userId,fid);
        m_pSql->UpdateMysql(szsql);
    }
}
