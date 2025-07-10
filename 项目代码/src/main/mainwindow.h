#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "src/aichat/aichat.h"
#include <QKeyEvent>
#include "src/yolo_detect/objectdetect.h"
#include "src/yolo_seg/yoloseg.h"
#include <QImage>
#include<QCamera>
#include "multilightsensor.h" // 多路重量传感器头文件
#include "detectlabel.h" // 添加DetectLabel头文件
#include "tablemanager.h" // 添加TableManager头文件
#include <QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void sendChatInfo(QString,int status);
    void clickedSend();
    void cleanTextContent();
    void keyPressEvent(QKeyEvent *event) override;
    void Objectstyle();
    void setParValue(int);
    void startDetect();
    void showImg(QImage img);
    void clickedModel();
    void on_pushButton_5_clicked();
signals:
    void setTextEdit(QString,int status);
private slots:
    //void on_pushButton_5_clicked();

private:
    Ui::MainWindow *ui;
    ObjectDetect * detect;
    YoloSeg *  seg;
    MultiLightSensor *m_multiLightSensor; // 多路重量传感器
    DetectLabel *m_detectLabel; // 检测标签成员
    TableManager *tableMgr; // 表格管理器
    QTimer *clearTableTimer; // 10秒无标签清空表格
};
#endif // MAINWINDOW_H
