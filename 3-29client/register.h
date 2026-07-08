#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include "packdef.h"
#include "./Kernel/kernel.h"
#include "QMessageBox"
#include <QString>

namespace Ui {
class Register;
}

class Register : public QWidget
{
    Q_OBJECT

public:
    explicit Register(IKernel* pKernel,QWidget *parent = nullptr);
    ~Register();

private slots:
    void on_registerbtn_clicked();

private:
    Ui::Register *ui;
    IKernel* m_pKernel;
};

#endif // REGISTER_H
