#include "Community.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpPart>
#include <QFileDialog>
#include <QtWidgets/QApplication>
/**根据选中的节点获取所有的父节点
 * @brief MainWindow::selectNodeText
 * @param item
 * @return
 */
QVector <QString> selectNodeText(QTreeWidgetItem* item){
    QVector<QString>vector;
    QString varItem = item->text(0);
    QTreeWidgetItem * parentItem =item->parent();
    if(parentItem ==nullptr){ //第一种可能没有父节点
        vector.push_back(varItem);
        return vector;
    }
    QString path=parentItem->text(0);  //第二种可能有父节点1个
    vector.push_back(varItem);
    vector.push_back(path);
    while(parentItem!=nullptr){
        parentItem = parentItem->parent();
        if(parentItem ==nullptr){
            break;
        }
        vector.push_back(parentItem->text(0));
    }
    return vector;
}

void messageInfo(int type)
{
    if(type==1){
        QMessageBox::information(nullptr,"小提示","请先新建实验");
    }
    if(type==2){
        QMessageBox::information(nullptr,"小提示","当前节点无法操作");
    }
    if(type==3){
        QMessageBox::information(nullptr,"小提示","请先选中节点");
    }
    if(type==4){
        QMessageBox::information(nullptr,"小提示","请注意输入规则");
    }
    if(type==5){
        QMessageBox::information(nullptr,"小提示","已经存在，不允许创建");
    }
    if(type==6){
        QMessageBox::information(nullptr,"小提示","请选择模型");
    }
    if(type==7){
        QMessageBox::information(nullptr,"小提示","识别完成");
    }
    if(type==8){
        QMessageBox::information(nullptr,"小提示","没项目不能提交");
    }
    if(type==9){
        QMessageBox::information(nullptr,"小提示","请选中一行数据");
    }
    if(type==10){
        QMessageBox::information(nullptr,"小提示","没有权限");
    }
    if(type==11){
        QMessageBox::information(nullptr,"小提示","服务器连接异常");
    }
    if(type==12){
        QMessageBox::information(nullptr,"小提示","当前状态不能还原");
    }
    if(type==13){
        QMessageBox::information(nullptr,"小提示","执行成功");
    }
    if(type==14){
        QMessageBox::information(nullptr,"小提示","请输入内容");
    }
    if(type==15){
        QMessageBox::information(nullptr,"小提示","没有检测到数据");
    }
    if(type==16){
        QMessageBox::information(nullptr,"小提示","执行成功");
    }if(type==17){
        QMessageBox::information(nullptr,"小提示","保存失败");
    }
    if(type==18){
            QMessageBox::information(nullptr,"小提示","服务端异常");
    }
    if(type==19){
            QMessageBox::information(nullptr,"小提示","用户或密码不能为空");
    }
    if(type==20){
            QMessageBox::information(nullptr,"小提示","没有可上传的数据");
    }
    if(type==21){
            QMessageBox::information(nullptr,"小提示","未检测到摄像头");
    }
    if(type==22){
        QMessageBox::information(nullptr,"小提示","更新程序失败");
    }
    if(type==23){
        QMessageBox::information(nullptr,"小提示","正在下载中!可在下载页面查询");
    }
}

/**
 * 创建文件夹，没有提示信息
 * @brief MainWindow::mkdir
 * @param path
 */
int mkdirNoMsg(QString path){
    QDir dir(path);
    if(dir.exists()){
        return -1;
    }
    dir.mkpath(".");//创建实验项目文件夹
    return 0;
}

/**创建文件夹
 * @brief MainWindow::mkdir
 * @param path
 */
int mkdir(QString path)
{
    QDir dir(path);
    if(dir.exists()){
        messageInfo(5);
        return -1;
    }
    dir.mkpath(".");//创建实验项目文件夹
}
/**创建文件
 * @brief MainWindow::createFile
 * @param path
 */
void createFile(QString path,QString content)
{
    QFile file(path);
    if(file.open(QIODevice::Append|QIODevice::Text)){
         QTextStream out(&file);
         out<<content;
    }
    file.close();
}
void delDir(QString path)
{
    QDir dir(path);
    if(dir.exists()){
        bool fag = dir.removeRecursively();
        if(fag){
            qDebug()<<"success ";
        }
    }
}

/**
 * 修改文件夹名称
 * @brief reDirName
 * @param path 修改前文件夹名称
 * @param pathNewFile 修改后文件夹名称
 */
void reDirName(QString path,QString pathNewFile){
    QDir dir(path);
    if(dir.exists()){
        bool fag = dir.rename(path,pathNewFile);
        if(fag){
            qDebug()<<"success ";
        }else{
            qDebug()<<path<<"fail ";
        }
    }else{
        qDebug()<<path<<"不是文件加？";
    }
}

void delFile(QString path){
    QFile file(path);
    if(file.exists()){
        bool fag =  file.remove();
        if(fag){
            qDebug()<<"success ";
        }
    }
}

void reFileName(QString path,QString name){
    QFile file(path);
    if(file.exists()){
        bool fag = file.rename(name);
        if(fag){
           qDebug()<<"success ";
        }else{
           qDebug()<<path<<"fail？";
        }
    }else{
        qDebug()<<path<<"不是文件加？";
    }
}

//************************************
// 方法名称:	copyFolder
// 概要:		复制文件夹
// 返回值:		bool
// 参数:		const QString & fromDir 原路径
// 参数:		const QString & toDir 新路径
// 参数:		bool coverFileIfExist 如果存在是否覆盖
//************************************
bool copyFolder(const QString& fromDir, const QString& toDir, bool coverFileIfExist){
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if (!targetDir.exists()){    //如果目标目录不存在，则进行创建
        if (!targetDir.mkdir(targetDir.absolutePath())) return false;
    }
    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") continue;
        if (fileInfo.isDir()){// 当为目录时，递归的进行copy
            if (!copyFolder(fileInfo.filePath(),
                targetDir.filePath(fileInfo.fileName()),
                coverFileIfExist))
                return false;
        }else{   //当允许覆盖操作时，将旧文件进行删除操作
            if (coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }
            // 进行文件拷贝
            if (!QFile::copy(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()))){
                return false;
            }
        }
    }
    return true;
}

/**
 * 复制文件
 * @brief filePath
 * @param copyFilePath
 * @param destinationPath
 * @return
 */
bool copyFile(QString filePath,QString copyFilePath) {
//      string filePath = "/Users/aaa/Documents/C++File/a.jpg";
//      string copyFilePath = "/Users/aaa/Documents/C++File/a_copy.jpg";
        FILE* file = fopen(filePath.toStdString().data(),"rb");
        if(!file){
           printf("文件不存在:%s\n",filePath.toStdString().data());
           exit(0);
        }
        FILE* fileCopy = fopen(copyFilePath.toStdString().data(), "wb");
        //方式三:可以拷贝文本或者二进制文件(指图片、音视频文件)
        int buff[1024];
        int len;
        while ((len = fread(buff, sizeof(int), sizeof(buff)/sizeof(int) , file)) != 0) {
           fwrite(buff, sizeof(int) , len, fileCopy);
        }
        fclose(file);
        fclose(fileCopy);
        return true;
}

//图片名称生成编码
QString randomUtil(){
    QString rand = QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+QString::number(QRandomGenerator::global()->bounded(1000,100000));
    return rand;
}

/**
 * 硬件序列号获取
 * @brief on_readyReadStandardOutput
 */
QString readDeviceId(){
    QFile file("/proc/cpuinfo");
    /* 判断文件是否存在 */
    if (file.exists() ) {
        /* 以只读的方式打开 */
        file.open(QFile::ReadOnly);
        /* 以字符串的方式保存读出的结果 */
        QString deviceInfo = QLatin1String(file.readAll());
        deviceInfo.replace("\r\n","");
        deviceInfo.replace("\n","");
        deviceInfo.replace("\t","");
        QStringList list = deviceInfo.split(":");
        /* 关闭文件 */
        file.close();
        return list[list.size()-1].trimmed();
    }
    return "";
}




bool overwiteBinaryFile(QString filePath,QByteArray data){

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        return false;
    }
    qint64 bytesWritten = file.write(data);
    if(bytesWritten<=0){
        return false;
    }
    file.close();
    return true;
}

QString  modelPushBtnClick(){
    QString paht =QDir::currentPath();
    QString dlgTitle="请选择一个模型文件";
    QString filter ="模型文件(*.rknn);;";
    QString fileName = QFileDialog::getOpenFileName(nullptr,dlgTitle,paht,filter);
    return fileName;
}

QString videoPushBtnClick(){
    QString paht =QDir::currentPath();
    QString dlgTitle="请选择一个视频文件";
    QString filter ="rknn文件(*.avi *.mp4)";
    QString fileName = QFileDialog::getOpenFileName(nullptr,dlgTitle,paht,filter);
    return fileName;
}
QString imagePushBtnClick(){
    QString paht =QDir::currentPath();
    QString dlgTitle="请选择一个视频文件";
    QString filter ="rknn文件(*.png *.jpg)";
    QString fileName = QFileDialog::getOpenFileName(nullptr,dlgTitle,paht,filter);
    return fileName;
}
