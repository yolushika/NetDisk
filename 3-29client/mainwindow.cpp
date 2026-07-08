#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pKernel = new Kernel;
    if(m_pKernel->Connect()){
        qDebug()<<"open success";
    }else{
        QMessageBox::critical(this,"result","connect error");
    }
    m_pLogin = new login(m_pKernel);
    m_pLogin->setWindowTitle("登录");
    m_pLogin->setWindowIcon(QIcon(":/cs2.jpg"));
    m_pLogin->show();
    m_pos = 0;
    this->setWindowTitle("网盘");
    this->setWindowIcon(QIcon(":/cs2.jpg"));
    connect((Kernel*)m_pKernel,&Kernel::LoginRs,this,&MainWindow::LoginRs,Qt::BlockingQueuedConnection);
    connect((Kernel*)m_pKernel,&Kernel::GetFileListRs,this,&MainWindow::GetFileListRs,Qt::BlockingQueuedConnection);
    connect((Kernel*)m_pKernel,&Kernel::UploadFileInfoRs,this,&MainWindow::UploadFileInfoRs,Qt::BlockingQueuedConnection);
    connect((Kernel*)m_pKernel,&Kernel::Select,this,&MainWindow::Select,Qt::BlockingQueuedConnection);
    connect((Kernel*)m_pKernel,&Kernel::SharedLink,this,&MainWindow::SharedLinkRs,Qt::BlockingQueuedConnection);
    connect((Kernel*)m_pKernel,&Kernel::GetLink,this,&MainWindow::GetLinkRs,Qt::BlockingQueuedConnection);
    connect((Kernel*)m_pKernel,&Kernel::DownloadFile,this,&MainWindow::DownloadFile,Qt::BlockingQueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_pKernel;
    delete m_pLogin;
}

void MainWindow::showEvent(QShowEvent *event)
{
    ui->tableWidget->setColumnCount(3);
    //给文件设置表头
    QStringList lst;
    lst<<"文件名"<<"文件大小"<<"上传时间";
    ui->tableWidget->setHorizontalHeaderLabels(lst);
    //使表格平铺
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStyleSheet("background-color:transparent");
}

string MainWindow::fileDigest(const string &file)
{
    ifstream in(file.c_str(), std::ios::binary);
    if (!in)
        return "";

    MD5 md5;
    std::streamsize length;
    char buffer[1024];
    while (!in.eof()) {
        in.read(buffer, 1024);
        length = in.gcount();
        if (length > 0)
            md5.update(buffer, length);
    }
    in.close();
    return md5.toString();
}

void MainWindow::LoginRs(const char *szbuf)
{
    STRU_LOGIN_RS* sls = (STRU_LOGIN_RS*)szbuf;
    if(sls->szResult == _login_res_failde){
        QMessageBox::warning(this,"登录结果","用户或密码有误");
    }else if(sls->szResult == _login_res_noexist){
        QMessageBox::information(this,"登录结果","用户不存在");
    }else{
        //登录成功 登录窗口隐藏 mainwindow显示
        m_pLogin->hide();
        this->show();
        Id = sls->szUserId;
        //记录userId 以便后续获取文件列表操作
        STRU_GETFILELIST_RQ sgr;
        sgr.szUserId=Id;
        m_pKernel->SendData((char*)&sgr,sizeof(sgr));
    }
}

void MainWindow::GetFileListRs(const char *szbuf)
{
    STRU_GETFILELIST_RS* sgs = (STRU_GETFILELIST_RS*)szbuf;
    for(int i=0;i<sgs->szFileNum;i++){
        int nRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nRow);

        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QIcon(":/.png"),sgs->arrFileInfo[i].szFileName));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(sgs->arrFileInfo[i].szFileSize)));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(sgs->arrFileInfo[i].szFileUploadTime));
    }
}

void MainWindow::UploadFileInfoRs(const char *szbuf)
{
    //根据上传文件信息进行下一步操作
    STRU_UPLOADFILEINFO_RS* sur = (STRU_UPLOADFILEINFO_RS*)szbuf;
    UploadFileInfo* pInfo = NULL;
    auto ite = m_lstUploadFileInfo.begin();
    while(ite != m_lstUploadFileInfo.end()){
        if(strcmp(sur->szFileMD5,(*ite)->szFileMD5)==0){
            pInfo = *ite;
            break;
        }
        ite++;
    }
    //遍历链表以确定下一步操作
    switch(sur->m_Result){
    case _uploadfileinfo_repeat:
        QMessageBox::warning(this,"提示","文件已经上传过了");
        return;
        break;
    case _uploadfileinfo_flashtrans:
    {
        QMessageBox::information(this,"提示","上传成功");
        break;
    }
    case _uploadfileinfo_contuine:
        //断点续传
    case _uploadfileinfo_normal:
        //需要发送的文件内容
        //打开文件
        STRU_UPLOADFILECONTENT_RQ scr;
        scr.FileId = sur->fileId;
        scr.UserId = Id;
        FILE *pFile = fopen(pInfo->szFilePath,"rb");
        if(sur->m_Result==_uploadfileinfo_contuine){
            fseek(pFile,sur->m_Pos,SEEK_SET);
        }
        //读取文件内容并发送
        while(1){
            size_t readNum = fread(scr.m_FIleContent,sizeof(char),sizeof(scr.m_FIleContent),pFile);
            if(readNum>0){
                scr.m_FileNum = readNum;
                m_pKernel->SendData((char*)&scr,sizeof(scr));
            }else{
                break;
            }
        }
        //关闭文件
        fclose(pFile);
        //将文件信息显示在窗口上
        break;
    }
    //将上传的文件显示在表格
    int nRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(nRow);

    QString strFileName = QString::fromStdString(string(pInfo->szFilePath)).section('/',-1);
    ui->tableWidget->setItem(nRow,0,new QTableWidgetItem(QIcon(":/.png"),strFileName));
    ui->tableWidget->setItem(nRow,1,new QTableWidgetItem(QString::number(pInfo->m_szFileSize)));
    ui->tableWidget->setItem(nRow,2,new QTableWidgetItem(pInfo->szFileUploadTime));
    delete pInfo;
    m_lstUploadFileInfo.erase(ite);
}

void MainWindow::Select(const char *szbuf)
{
    STRU_SELECTFILE_RS* ssr = (STRU_SELECTFILE_RS*)szbuf;

    for(int i=0;i<ssr->szFileNum;i++){
        int nRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nRow);

        ui->tableWidget->setItem(nRow,0,new QTableWidgetItem(QIcon(":/.png"),ssr->arrFileInfo[i].szFileName));
        ui->tableWidget->setItem(nRow,1,new QTableWidgetItem(QString::number(ssr->arrFileInfo[i].szFileSize)));
        ui->tableWidget->setItem(nRow,2,new QTableWidgetItem(ssr->arrFileInfo[i].szFileUploadTime));
    }
}

void MainWindow::SharedLinkRs(const char *szbuf)
{
    STRU_SHARELINK_RS* pssr = (STRU_SHARELINK_RS*)szbuf;
    QString str = QString("分享文件：").append(pssr->szFileName).append("成功！提取码是：").append(pssr->szCode);
    QMessageBox::information(this,"分享文件",str);
}

void MainWindow::GetLinkRs(const char *szbuf)
{
    STRU_GETLINK_RS* psgr = (STRU_GETLINK_RS*)szbuf;
    //判断服务器处理的结果
    if(psgr->szResult == _getlink_failed){
        //失败 弹出提示框
        QMessageBox::warning(this,"结果","此文件你已拥有请不要重复提取");
    }else{
        //成功 将文件信息追加进表格
        //表头设置
        int nRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(nRow);

        ui->tableWidget->setItem(nRow,0,new QTableWidgetItem(QIcon(":/.png"),psgr->szFileName));
        ui->tableWidget->setItem(nRow,1,new QTableWidgetItem(QString::number(psgr->szFileSize)));
        ui->tableWidget->setItem(nRow,2,new QTableWidgetItem(psgr->szFileUploadTime));
    }
}

void MainWindow::DownloadFile(const char *szbuf)
{
    STRU_DOWNLOADFILE_RS* sds = (STRU_DOWNLOADFILE_RS*)szbuf;
    //保存的文件路径
    FILE* pfile = fopen(filepath.c_str(),"ab");
    fseek(pfile,m_pos,SEEK_SET);
    int WriteNum = fwrite(sds->m_FileContent,1,sds->m_fileNum,pfile);
    if(WriteNum > 0){
        m_pos += WriteNum;
    }
    fclose(pfile);
}

void MainWindow::on_action_triggered()
{
    //qDebug()<<"上传文件被点击了";
    //获取要上传的文件信息
    QString filePath = QFileDialog::getOpenFileName(this, tr("打开文件"),
                                                    "D:\\Qt project\\Netdisk-UserFiles",//起始路径
                                                    tr("All Files (*.*);;Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"));
    //printf("%s",filePath.toStdString().c_str());
    //分割出文件名————字符串分割
    //qDebug()<<filePath.section('/',-1);
    QString fileName = filePath.section('/',-1);
    if(fileName==""){
        return;
    }
    //文件大小 QFile类中的size()
    QFile qfile(filePath);
    qfile.open(QIODevice::ReadOnly);
    qint64 filesize = qfile.size();
    qfile.close();
    //MD5值
    string strMD5 = fileDigest(filePath.toStdString());//MD5算法，用于获取文件的唯一标识，来判断文件是否已存在，从而判断需要妙传否
    //qDebug()<<QString::fromStdString(strMD5);
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd HH:mm:ss");
    //qDebug()<<strTime;

    //发送上传文件信息请求————告诉服务器我要上传哪个文件
    STRU_UPLOADFILEINFO_RQ sur;
    strcpy(sur.szFileMD5,strMD5.c_str());
    strcpy(sur.szFileName,fileName.toStdString().c_str());
    strcpy(sur.szFileUploadTime,strTime.toStdString().c_str());
    sur.szfileSize = filesize;
    sur.UserId = Id;

    //收到服务器准许上传文件内容的回复时 如何知道客户端要传输哪个文件？
    //记录文件信息 文件名 路径 文件ID 文件大小
    UploadFileInfo *pInfo = new UploadFileInfo;
    pInfo->m_szFileSize = filesize;
    strcpy(pInfo->szFileUploadTime,strTime.toStdString().c_str());
    strcpy(pInfo->szFilePath,filePath.toStdString().c_str());
    strcpy(pInfo->szFileMD5,strMD5.c_str());
    pInfo->m_pos = 0;
    m_lstUploadFileInfo.push_back(pInfo);

    m_pKernel->SendData((char*)&sur,sizeof(sur));
}


void MainWindow::on_select_clicked()
{
    ui->tableWidget->setRowCount(0);
    STRU_SELECTFILE_RQ ssr;
    QString KeyWord = ui->lineEdit->text();
    strcpy(ssr.m_KeyWord,KeyWord.toStdString().c_str());
    ssr.UserId = Id;
    m_pKernel->SendData((char*)&ssr,sizeof(ssr));
}


void MainWindow::on_Delete_clicked()
{
    //用户点击表格内的一行，删除这个文件  根据文件名删除
    //获取文件名
    int nRow = ui->tableWidget->currentRow();
    if(nRow<0)
        return;
    QString fileName = ui->tableWidget->item(nRow,0)->text();

    STRU_DELETEFILE_RQ sdr;
    sdr.userid = Id;
    strcpy(sdr.szFileName,fileName.toStdString().c_str());
    if(m_pKernel->SendData((char*)&sdr,sizeof(sdr))){
        QMessageBox::information(this,"删除文件","此文件已成功删除");
        ui->tableWidget->removeRow(nRow);
    }
}

//分享文件
void MainWindow::on_action_3_triggered()
{
    int nRow = ui->tableWidget->currentRow();
    if(nRow<0)
        return;
    QString fileName = ui->tableWidget->item(nRow,0)->text();
    STRU_SHARELINK_RQ ssr;
    ssr.userid = Id;
    strcpy(ssr.szFileName,fileName.toStdString().c_str());
    m_pKernel->SendData((char*)&ssr,sizeof(ssr));
}


void MainWindow::on_action_4_triggered()
{
    STRU_GETLINK_RQ sgr;
    //弹出对话框
    m_pDialog = new Dialog(this);
    int Result = m_pDialog->exec();
    //判断对话框点击的结果
    if(Result == QDialog::Accepted){
        QDateTime time = QDateTime::currentDateTime();
        QString strTime = time.toString("yy-MM-dd HH:mm:ss");
        strcpy(sgr.szCode,m_pDialog->m_pCode.c_str());
        sgr.userId = Id;
        strcpy(sgr.szFileUploadTime,strTime.toStdString().c_str());
    }
    m_pKernel->SendData((char*)&sgr,sizeof(sgr));
}


void MainWindow::on_action_2_triggered()
{
    STRU_DOWNLOADFILE_RQ sdq;
    //先获取用户信息
    int nRow = ui->tableWidget->currentRow();
    if(nRow < 0)
        return;
    filepath = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    "./",
                                                    "Images (*.png *.xpm *.jpg);;Text files (*.txt);;"
                                                    "XML files (*.xml)").toStdString();

    //文件名和用户Id组合发给服务器
    string fileName = ui->tableWidget->item(nRow,0)->text().toStdString();
    sdq.userId = Id;
    strcpy(sdq.szFileName,fileName.c_str());
    m_pKernel->SendData((char*)&sdq,sizeof(sdq));
}

