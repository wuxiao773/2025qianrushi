#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv2/opencv.hpp"
#include <QDateTime>
#include <QTextTable>
#include <QTextTableCell>
AiChat *acht;
using namespace std;
int yoloStatus=0;    //0表示yolov5 1 表示yolov8
int videoStatus=0;   //0表示摄像头 1表示视频  2 表示图片
QString videoPath;   //识别视频文件路径
int rangingStatus=-1;//状态:标识是否测距
QString modelPath;   //模型文件路径
bool runStatus=false;//控制运行线程while
int objVideoNode;    //摄像头输入编号
string objLabelPath="coco_80_labels_list.txt"; //类别文件路径
string segLabelPath="coco_80_labels_list.txt"; ;
int strackStatus=0;
int yolo_seg_status=0;      //0表示yolov5 1 表示yolov8
QString videoPath1;   //识别视频文件路径
int rangingStatus1;   //状态:标识是否测距
QString modelPath1;   //模型文件路径
bool runStatus1;      //控制运行线程while
int objVideoNode1;    //摄像头输入编号：v4l2-ctl --list-devices查看系统设备“USB Camera: USB Camera”的设备/dev/video20后缀20
int strackStatus1; //是否开启跟踪


 QString videoPath2;   //识别视频文件路径
 int rangingStatus2;   //状态:标识是否测距
 QString modelPath2;   //模型文件路径
 bool runStatus2;      //控制运行线程while
 int objVideoNode2;    //摄像头输入编号：v4l2-ctl --list-devices查看系统设备“USB Camera: USB Camera”的设备/dev/video20后缀20
 int strackStatus2; //是否开启跟踪
 using namespace cv;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Objectstyle();
    QIcon ic(":/AI.png");
    this->setWindowIcon(ic);
    acht = new AiChat(this);
    detect = new ObjectDetect;;
    seg = new YoloSeg;
    connect(acht,&AiChat::sendChatInfo, this, &MainWindow::sendChatInfo);
    connect(acht,&AiChat::setParValue, this, &MainWindow::setParValue);
    connect(ui->pushButton,&QPushButton::clicked, this, &MainWindow::clickedSend);
//    connect(this,&MainWindow::setTextEdit, this, &MainWindow::sendChatInfo);
    connect(ui->pushButton_2,&QPushButton::clicked, this, &MainWindow::cleanTextContent);
    connect(ui->pushButton_4,&QPushButton::clicked, this, &MainWindow::startDetect);
     connect(ui->pushButton_3,&QPushButton::clicked, this, &MainWindow::clickedModel);
    connect(detect,&ObjectDetect::showImg, this, &MainWindow::showImg);
    connect(seg,&YoloSeg::showImg, this, &MainWindow::showImg);
    connect(ui->pushButton_5,&QPushButton::clicked, this, &MainWindow::on_pushButton_5_clicked);

    tableMgr = new TableManager(ui->tableWidget, this);

    // 10秒无标签清空表格定时器
       clearTableTimer = new QTimer(this);
       clearTableTimer->setInterval(10000); // 10秒
       connect(clearTableTimer, &QTimer::timeout, this, [=]{
           for (int i = 0; i < 4; ++i) {
               tableMgr->setRowData(i, "", 0);
           }
       });
       clearTableTimer->start();

       // 多路重量传感器初始化
       m_multiLightSensor = new MultiLightSensor(this);
       static int sensorValues[4] = {0};
       connect(m_multiLightSensor, &MultiLightSensor::valuesChanged, this, [=](int v1, int v2, int v3, int v4){
           sensorValues[0] = v1; // 左上
           sensorValues[1] = v2; // 右上
           sensorValues[2] = v3; // 右下
           sensorValues[3] = v4; // 左下
           // 可在此处联动UI显示重量值
       });
       m_multiLightSensor->start();
        // 检测标签初始化
          m_detectLabel = new DetectLabel(this);
           connect(detect, &ObjectDetect::detectLabel, this, [=](const QString &label, const QString &pos_label){
                   ui->label_7->setText(QString("检测标签: %1").arg(label));
              clearTableTimer->start(); // 每次检测到标签都重置定时器
              if (label.isEmpty()) {
                // 没有检测到标签，清空所有表格行
                for (int i = 0; i < 4; ++i) {
                    tableMgr->setRowData(i, "", 0);
                }
                return;
            }

            int row = -1;
            if (pos_label == "leftup") row = 0;
            else if (pos_label == "rightup") row = 1;
            else if (pos_label == "rightdown") row = 2;
            else if (pos_label == "leftdown") row = 3;
            if (row >= 0 && row < 4) {
                // 清空该行
                tableMgr->setRowData(row, "", 0);
                // 写入新标签和重量值
                tableMgr->setRowData(row, label, sensorValues[row]);
            }
          });


          // 连接目标检测信号到标签管理
          connect(detect, &ObjectDetect::detectLabel, m_detectLabel, &DetectLabel::setLabel);
}

MainWindow::~MainWindow()
{
    acht->quit();
    delete ui;
}

void MainWindow::Objectstyle()
{
    ui->progressBar->hide();
    ui->pushButton->setEnabled(false);
     ui->pushButton->setStyleSheet("background-color:#166abd");
      ui->pushButton_3->setStyleSheet("background-color:#15aaaa;color:#FFFFFF");
      ui->pushButton_4->setObjectName("startBtn");
    ui->label->hide();
    ui->label_2->hide();
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setObjectName("textEditCss");
    ui->lineEdit->setStyleSheet("border-radius: 8px;height:20px");
    ui->lineEdit_2->setObjectName("textEditCss_2");
     ui->lineEdit_2->setStyleSheet("border-radius: 8px;height:20px;");
    ui->pushButton_2->setObjectName("clear");
    ui->pushButton_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->pushButton_2->setMaximumWidth(70);

    ui->pushButton->setObjectName("send");
    ui->pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->pushButton->setMaximumWidth(70);
//    ui->pushButton->setStyleSheet("background-color: #15aaaa;");
    QFile file(":/chat.qss");
       /* 判断文件是否存在 */
       if (file.exists() ) {
           /* 以只读的方式打开 */
           file.open(QFile::ReadOnly);
           /* 以字符串的方式保存读出的结果 */
           QString styleSheet = QLatin1String(file.readAll());
           /* 设置全局样式 */
           qApp->setStyleSheet(styleSheet);
           /* 关闭文件 */
           file.close();
       }


}

void MainWindow::setParValue(int value)
{
  ui->progressBar->show();
  ui->label->show();
   ui->progressBar->setFormat("Loading: %p%");
  ui->progressBar->setValue(value);
  if(ui->progressBar->value()==80){
     ui->label->setText("连接成功");

  }
  if(ui->progressBar->value()==100){
      QThread::sleep(1);
       ui->label->hide();
      ui->pushButton_2->setText("清空");
      ui->progressBar->hide();

  }
}
#include <QMessageBox>
#include <QCameraInfo>
#include<QCamera>
//视觉检测启动
void MainWindow::startDetect(){
    if(ui->lineEdit_2->text().isEmpty()||ui->lineEdit_2->text().trimmed()==""){
        QMessageBox::information(nullptr,"小提示","模型未选");
        return;
    }
    qDebug()<<"摄像头打开中请稍等";
     modelPath =ui->lineEdit_2->text();   //模型文件路径
     runStatus=true;//控制运行线程while
    if(ui->pushButton_4->text()=="启动"){
        ui->comboBox->setEnabled(false);
        ui->comboBox_2->setEnabled(false);
        yoloStatus= ui->comboBox->currentIndex(); //yolo版本 0:v5 1:v8
        int yoloType= ui->comboBox_2->currentIndex(); //检测类型  0 目标检测 1 实力分割 2 旋转框
        runStatus =true;
        if(yoloType==0){
        detect->start();
        }else if(yoloType==1){
        seg->start();
        }else if(yoloType==2){
            QMessageBox::information(nullptr,"小提示","功能暂未开发");
            return;
        }
       ui->pushButton_4->setText("停止");
    }else if(ui->pushButton_4->text()=="停止"){
        int yoloType= ui->comboBox_2->currentIndex();
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        runStatus =false;
        if(yoloType==0){
         detect->quit();
         detect->wait();
        }else if(yoloType==1){
            seg->quit();
            seg->wait();
        }else if(yoloType==2){

        }
        ui->pushButton_4->setText("启动");
    }
}

void MainWindow::showImg(QImage img)
{
    // 保持比例铺满且无黑边
       QPixmap pixmap = QPixmap::fromImage(img).scaled(
           ui->label_3->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
       QRect targetRect(0, 0, ui->label_3->width(), ui->label_3->height());
       QPixmap cropped = pixmap.copy(targetRect & pixmap.rect());
       ui->label_3->setPixmap(cropped);
    // ui->label_3->setPixmap(QPixmap::fromImage(img));
}



#include <QtDebug>
QString date;


void MainWindow::sendChatInfo(QString chatInfo,int status)
{
    if("<think>"==chatInfo||"</think>"==chatInfo||"\n\n"==chatInfo){
        return;
    }

    QString content = ui->textEdit->toPlainText()+chatInfo;
    if(chatInfo=="robot: "){

        ui->textEdit->append("\n");
        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QTextCursor cursor = ui->textEdit->textCursor();
        // 插入一个表格，表格有 2 行 2 列
        QTextTableFormat tableFormat;
        tableFormat.setBorder(0);
        QTextBlockFormat ormat;
        ormat.setLineHeight(1.5, QTextBlockFormat::ProportionalHeight);  // 这里设置了行高，可以调整
        cursor.setBlockFormat(ormat);
        tableFormat.setAlignment(Qt::AlignLeft);  // 设置表格居中
        QTextTable *table = cursor.insertTable(2, 2, tableFormat);
        table->mergeCells(0, 0, 2, 1);
        QTextBlockFormat blockFormat;
        blockFormat.setAlignment(Qt::AlignCenter);  // 设置内容居中
        // 获取合并后的单元格
         QTextCursor  cursor2= table->cellAt(0,0).firstCursorPosition();
        cursor2.setBlockFormat(blockFormat);

        // 设置合并单元格的对齐方式为居中


         QTextCursor cellCursor = table->cellAt(0, 0).firstCursorPosition();
         QTextImageFormat imageFormat;
        imageFormat.setWidth(50);  // 设置图标宽度
        imageFormat.setHeight(40); // 设置图标高度
        imageFormat.setName(":/AI.png");  // 图标路径
        cellCursor.insertImage(imageFormat);
        cellCursor = table->cellAt(0, 1).firstCursorPosition();
        cellCursor.insertText(currentTime);
        cellCursor = table->cellAt(1,1).firstCursorPosition();
        date = currentTime;
    }else{
        QTextDocument *doc = ui->textEdit->document();
        QTextCursor cursor(doc);
        QTextTable *lastTable = nullptr;

        // 遍历文档中的所有表格
        while (!cursor.atEnd()) {
            if (cursor.currentTable()) {
                QTextTable *table = cursor.currentTable();

                // 检查表格中是否包含标识文本
                QTextCursor tableCursor = table->cellAt(0, 1).firstCursorPosition();
                QString tableText = tableCursor.block().text();
                if (tableText==date) {
//                    qDebug()<<tableText;
                    lastTable = table;  // 找到标识的表格
                     int rowCount = table->rows();
                     QTextCursor cellCursor = table->cellAt(rowCount - 1, 1).firstCursorPosition();
                     // 确保光标被设置到目标单元格
                     if (!cellCursor.isNull()) {
                         qDebug()<<chatInfo;
                         cellCursor.movePosition(QTextCursor::EndOfBlock); //让光标在当前单元格最后
                         if(chatInfo.endsWith("\n")){
                             qDebug()<<"1----------";
                             chatInfo+="<br>";
                         }
                         cellCursor.insertHtml("<div style ='background-color: rgb(254,246, 248);'>"+chatInfo+"</div>");

//                       qDebug()<<  cellCursor.block().text();
                       ui->textEdit->moveCursor(QTextCursor::End);
                     }
                     break;
                }
            }
            cursor.movePosition(QTextCursor::NextBlock);
        }
       if(status==2){
           ui->pushButton->setEnabled(true);
           ui->pushButton_2->setEnabled(true);
            ui->pushButton->setStyleSheet("background-color:#1c97f5");
       }

    }
}

void MainWindow::clickedSend()
{


    QString chat =  ui->lineEdit->text().trimmed();
    if(chat.isEmpty()){
        return;
    }
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setStyleSheet("background-color:#166abd");
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    QTextCursor cursor = ui->textEdit->textCursor();

    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignRight);  // 设置时间居中
    cursor.setBlockFormat(blockFormat);
    cursor.insertText(currentTime);
    ui->textEdit->setTextCursor(cursor);
    cursor.insertBlock();
    cursor = ui->textEdit->textCursor();
    QTextCharFormat formatText;
    // 自定义背景颜色，使用 RGB 值 (例如 RGB(255, 255, 0) 为黄色)
    QColor backgroundColor(218, 242, 212);  // RGB(255, 255, 0) 为黄色
    formatText.setBackground(backgroundColor);
    cursor.mergeCharFormat(formatText);
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignRight);  // 设置右对齐
    //        ui->textEdit->append("user："+ui->lineEdit->text());
    cursor.insertText(ui->lineEdit->text());

    cursor.setBlockFormat(format);
    ui->textEdit->setTextCursor(cursor);

    //    cursor.insertText("user："+ui->lineEdit->text()+"\n");
    acht->chat=chat;
    acht->start();
    ui->lineEdit->clear();
}

void MainWindow::cleanTextContent()
{
    if(ui->pushButton_2->text()=="连接"){
     ui->pushButton_2->setEnabled(false);
     ui->pushButton_2->setStyleSheet("background-color:#15aaaa");
     acht->AiInit();
     ui->pushButton_2->setEnabled(true);
     ui->pushButton->setEnabled(true);
      ui->pushButton->setStyleSheet("background-color:#1c97f5");
    }else{
    ui->textEdit->clear();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{

    if(ui->pushButton->isEnabled()==false){
        return;
    }
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        clickedSend();
    } else {
        QWidget::keyPressEvent(event);
    }
}
#include <QDir>
#include <QFileDialog>
void MainWindow::clickedModel()
{
    QString paht =QDir::currentPath();
    QString dlgTitle="请选择一个模型文件";
    QString filter ="模型文件(*.rknn)";
    modelPath = QFileDialog::getOpenFileName(nullptr,dlgTitle,paht,filter);
    ui->lineEdit_2->setText(modelPath);
}



void MainWindow::on_pushButton_5_clicked()
{
    QStringList foodList;
    for(int i=0; i<ui->tableWidget->rowCount(); ++i) {
        QString label = ui->tableWidget->item(i, 0) ? ui->tableWidget->item(i, 0)->text() : "";
        QString value = ui->tableWidget->item(i, 1) ? ui->tableWidget->item(i, 1)->text() : "";
        if(!label.isEmpty() && !value.isEmpty()) {
            foodList << QString("%1 %2克").arg(label).arg(value);
        }
    }
    if(foodList.isEmpty()) {
        QMessageBox::information(this, "提示", "请先检测标签和传感器值");
        return;
    }
    // 构造自然语言问题
    QString question = QString("以下是我的食物：%1。请告诉我这些食物的总脂肪、总蛋白质、总热量，并给出简单的饮食建议，总字数不超过40字。")
        .arg(foodList.join("，"));
    acht->chat = question;
    acht->start();
}
