#ifndef COMMUNITY_H
#define COMMUNITY_H
#include <QString>
#include <queue>
#include <ostream>
#include <QDir>
#include <QtDebug>
#include <QFile>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QRandomGenerator>
#include "utils/image_utils.h"
#include "src/community/config.h"
#include <QProcess>

using namespace std;
//extern queue<unsigned char *>imageData;
extern int videoStatus;     //0表示摄像头 1表示视频 2表示图片
extern  string segLabelPath; //类别文件路径
QVector <QString> selectNodeText(QTreeWidgetItem* item);
void messageInfo(int type);
int mkdirNoMsg(QString);//创建文件夹，没有提示信息
int mkdir(QString);
void createFile(QString path,QString content);
void delDir(QString path);
void reDirName(QString path,QString pathNewFile);
void delFile(QString path);
void reFileName(QString path,QString name);
bool copyFolder(const QString& fromDir, const QString& toDir, bool coverFileIfExist);//复制文件夹
bool copyFile(QString filePath,QString copyFilePath);//复制文件
QString randomUtil();
QString readDeviceId();
bool overwiteBinaryFile(QString filePath,QByteArray data);
QString modelPushBtnClick();
QString videoPushBtnClick();
QString imagePushBtnClick();
extern int yoloStatus;      //0表示yolov5 1 表示yolov8
extern QString videoPath;   //识别视频文件路径
extern int rangingStatus;   //状态:标识是否测距
extern QString modelPath;   //模型文件路径
extern bool runStatus;      //控制运行线程while
extern int objVideoNode;    //摄像头输入编号：v4l2-ctl --list-devices查看系统设备“USB Camera: USB Camera”的设备/dev/video20后缀20
extern int strackStatus; //是否开启跟踪
//QProcess * terminal;
//void on_readyReadStandardOutput();//硬件序列号获取
#endif // COMMUNITY_H
