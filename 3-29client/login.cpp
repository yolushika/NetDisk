#include "login.h"
#include "ui_login.h"

login::login(IKernel* pKernel,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    m_pKernel = pKernel;
    connect((Kernel*)m_pKernel,&Kernel::RegisterRs,this,&login::RegisterRs,Qt::BlockingQueuedConnection);
}

login::~login()
{
    delete ui;
}

void login::on_loginbtn_clicked()
{
    //采集页面上的信息
    QString UserName = ui->username->text();
    QString PassWord = ui->userpwd->text();
    //封装成STRU_LOGIN_RQ形式的信息
    STRU_LOGIN_RQ slr;
    strcpy(slr.szName,UserName.toStdString().c_str());
    strcpy(slr.szPassword,PassWord.toStdString().c_str());
    //发送至服务器
    m_pKernel->SendData((char*)&slr,sizeof(slr));
}

void login::RegisterRs(const char *szbuf)
{
    STRU_REGISTER_RS* srs = (STRU_REGISTER_RS*)szbuf;
    if(srs->szResult == _register_res_failed){
        QMessageBox::information(this,"提示","用户名已被注册！");
    }else{
        QMessageBox::information(this,"提示","注册成功！");
        m_Register->hide();
        this->show();
    }
}


void login::on_registerbtn_clicked()
{
    //创建register窗口，Kernel传输
    m_Register = new Register(m_pKernel);
    this->hide();
    m_Register->show();
}

