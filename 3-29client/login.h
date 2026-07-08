#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "packdef.h"
#include "./Kernel/kernel.h"
#include "QMessageBox"
#include <QString>
#include "register.h"

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(IKernel* pKernel,QWidget *parent = nullptr);
    ~login();

private slots:
    void on_loginbtn_clicked();
    void RegisterRs(const char* szbuf);
    void on_registerbtn_clicked();

private:
    Ui::login *ui;
    IKernel* m_pKernel;
    Register* m_Register;
};

#endif // LOGIN_H
