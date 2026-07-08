#include <QCoreApplication>
#include "./Kernel/kernel.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    IKernel* pKernel = new Kernel;
    if(Kernel::GetKernel()->open()){
        cout<<"server is running"<<endl;
    }
//注册测试
    // STRU_REGISTER_RQ srr;
    // strcpy(srr.szName,"adu1");
    // strcpy(srr.szPassword,"123456");
    // srr.szTel = 12345678910;
    // Kernel::GetKernel()->dealData(NULL,(char*)&srr);
//登录测试
    // STRU_LOGIN_RQ slr;
    // strcpy(slr.szName,"adu1");
    // strcpy(slr.szPassword,"123456");
    // Kernel::GetKernel()->dealData(NULL,(char*)&slr);
    return a.exec();
}
