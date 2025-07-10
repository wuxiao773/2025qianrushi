//#ifndef CONFIG_H
//#define CONFIG_H
#ifndef GLOBAL_STRUCT_H
#define GLOBAL_STRUCT_H
#include <QSettings>
//https://blog.csdn.net/hyyjiushiliangxing/article/details/100899284
extern struct Config {//应用配置信息
    QString dbname;   //1、数据ku名称
    int videonum;     //2、摄像头编号：目前改为应用自动获取：VideoCapture cats(20);//20 0 v4l2-ctl --list-devices
    QString svsyndata;//3、服务器地址
    QString yoloPath; //4、应用程序路径
    QString version;// 本程序版本号=*.pro文件设置,VERSION=2.1
    QString upstartname; //更新启动程序名称
}cfg;

extern struct UserLogin {//用户信息
    QString accountid;   //1、帐号id
    QString account;     //2、帐号
    QString teamid;      //3、团队id
    QString level;       //4、帐号级别：0为平台管理员；1为团队管理员；2为团队用户角色查看自己实验数据做相应实验；
    QString username;
}userLogin;

//class config{
// public:
//    config();

//};
#endif // CONFIG_H

