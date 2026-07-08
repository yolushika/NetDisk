#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "login.h"
#include <QDebug>
#include <QMessageBox>
#include "Kernel/kernel.h"
#include "MD5/md5.h"
#include <QFileDialog>
#include <QDateTime>
#include "dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct UploadFileInfo{
    char szFilePath[FILE_PATH];  //客户端需要读取文件内容并发送
    char szFileUploadTime[MAX_SIZE];
    long long m_szFileSize;
    long long m_pos;    //偏移量
    char szFileMD5[MAX_SIZE];
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showEvent(QShowEvent *event);
    string fileDigest(const string &file);


private slots:
    void LoginRs(const char* szbuf);
    void GetFileListRs(const char* szbuf);
    void UploadFileInfoRs(const char* szbuf);
    void Select(const char* szbuf);
    void SharedLinkRs(const char* szbuf);
    void GetLinkRs(const char* szbuf);
    void DownloadFile(const char* szbuf);

    void on_action_triggered();

    void on_select_clicked();

    void on_Delete_clicked();

    void on_action_3_triggered();

    void on_action_4_triggered();

    void on_action_2_triggered();

private:
    Ui::MainWindow *ui;
    IKernel* m_pKernel;
    login* m_pLogin;
    long long Id;
    std::list<UploadFileInfo*> m_lstUploadFileInfo;
    Dialog* m_pDialog;
    string filepath;
    long long m_pos;
};
#endif // MAINWINDOW_H
