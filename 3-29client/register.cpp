#include "register.h"
#include "ui_register.h"

Register::Register(IKernel* pKernel,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Register)
{
    ui->setupUi(this);
    m_pKernel = pKernel;
    this->setWindowTitle("注册");
    this->setWindowIcon(QIcon(":/cs2.jpg"));
}

Register::~Register()
{
    delete ui;
}

void Register::on_registerbtn_clicked()
{
    //采集页面上的信息
    QString UserName = ui->username->text();
    QString PassWord = ui->userpwd->text();
    QString UPhone = ui->userPhone->text();

    if(ui->username->text()==NULL){
        QMessageBox::warning(this,"","用户名不可以为空");
        ui->username->setFocus();
        return;
    }
    //格式校验
    bool A=false,b=false,Num=false;
    for(int i=0;i<PassWord.size();i++){
        if(PassWord[i]>='0'&&PassWord[i]<='9'){
            Num=true;//存在数字
        }else if(PassWord[i]>='a'&&PassWord[i]<='z'){
            b=true;//存在小写字母
        }else if(PassWord[i]>='A'&&PassWord[i]<='Z'){
            A=true;//存在大写字母
        }
    }
    if(!(A && b && Num)){
        QMessageBox::warning(this,"","请输入合法的密码：包括大写字母A-Z,小写字母a-z，数字1-9");
        ui->userpwd->setFocus();
        return;
    }
    if(UPhone.toLongLong()<10000000000||UPhone.toLongLong()>99999999999){
        QMessageBox::warning(this,"","请输入合法的电话：电话号码为11位");
        ui->userPhone->setFocus();
        return;
    }
    //封装成STRU_Register_RQ形式的信息
    STRU_REGISTER_RQ srr;
    strcpy(srr.szName,UserName.toStdString().c_str());
    strcpy(srr.szPassword,PassWord.toStdString().c_str());
    srr.szTel = UPhone.toLongLong();
    //发送至服务器
    m_pKernel->SendData((char*)&srr,sizeof(srr));

}

