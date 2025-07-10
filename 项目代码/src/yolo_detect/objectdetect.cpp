#include "objectdetect.h"
#include "opencv2/opencv.hpp"
#include "yolo.h"
#include "utils/image_drawing.h"
#include "3rdparty/stb_image/timer.h"
#include <QMap>
#include <queue>
#include <vector>
#include <QCameraInfo>
#include "src/community/Community.h"
#include <iostream>
#include <cstdlib>
#include <QList>
using namespace cv;
ObjectDetect::ObjectDetect(QObject *parent): QThread{parent}{

}
void ObjectDetect::run(){
    TIMER ti;
    int ret;
    int max_track_time = 4;
    rknn_app_context_t rknn_app_ctx;
    memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t));

    ret = obj_init_post_process();
    if (ret != 0){
        printf("obj_init_post_process fail! ret=%d \n", ret);
        goto out;
        return;
    }
    //2、读取模型model_path文件内容，将模型初始化到rknn_app_context_t *rknn_app_ctx里面
    ret = init_yolo_model(modelPath.toStdString().c_str(), &rknn_app_ctx);
    if (ret != 0){
        printf("init_yolo_model fail! ret=%d \n", ret);
        goto out;
        return;
    }


    if(videoStatus ==0){
        //摄像头输入：v4l2-ctl --list-devices查看系统设备“USB Camera: USB Camera”的设备/dev/video20后缀20
        qDebug()<<"摄像头打开中请稍等";

        sleep(2);
        cv::VideoCapture cats("/dev/video21");
        if(!cats.isOpened()){
             qDebug()<<"摄像头wuxiao";
             qDebug()<<"cwu:"<<strerror(errno);
        }

        cats.set(cv::CAP_PROP_FRAME_WIDTH,640);//设置后无法：无法打开“单”摄像头节点?
        cats.set(cv::CAP_PROP_FRAME_HEIGHT,480);//设置后无法：无法打开“单”摄像头节点?
        //        cats.set(cv::CAP_PROP_FPS, 15);
        Mat fram;
        QMap<int,vector<cv::Point>*> mapId;
        QMap<int,int>idCount;
        // 定义价格数组（与类别顺序一致）
        const int PRICES[16] = {88, 38, 28, 32, 42, 36, 35, 30, 25, 40, 58, 48, 168, 46, 39, 45};

        while(runStatus){
            bool fag = cats.read(fram);
            if(fag==false){
                qDebug()<<"read is false";
                break;
            }
            if(fram.empty()){
                printf("fram is null");
                continue;
            }


            ti.tik();//获取开始时间start_time
            cv::Mat img;
            Mat limg;
            //cv::resize(fram,limg,Size(640,640));
            cv::copyMakeBorder(fram,limg,0,0,0,0,cv::BORDER_DEFAULT);
            cv::cvtColor(limg, img, cv::COLOR_BGR2RGB);//二值化获取视频对象图片fram=>img
            object_detect_result_list od_results;
            image_buffer_t src_image;
            memset(&src_image, 0, sizeof(image_buffer_t));
            src_image.width = 640;
            src_image.height = 480;
            src_image.format = IMAGE_FORMAT_RGB888;
            src_image.size = 640*480*3;
            unsigned char * data = (unsigned char *)img.data;
            src_image.virt_addr = data;//图片数据
            inference_yolo_model(&rknn_app_ctx, &src_image, &od_results,yoloStatus);
            if (od_results.count >= 1){

                char text[256];

                ti.tok();//获取结束时间stop_time
                ti.print_time("time===========");
                QList<DetectResultInfo> detectResults; // 用于保存所有目标信息
                int total_price = 0;
                for (int i = 0; i < od_results.count; i++)
                {
                    object_detect_result *det_result = &(od_results.results[i]);
                    int x1 = det_result->box.left;
                    int y1 = det_result->box.top;
                    int x2 = det_result->box.right;
                    int y2 = det_result->box.bottom;
                    int cx = (x1 + x2) / 2;
                    int cy = (y1 + y2) / 2;

                    // 计算位置标签
                    QString pos_label;
                    if (cx < 320 && cy < 240) pos_label = "leftup";
                    else if (cx >= 320 && cy < 240) pos_label = "rightup";
                    else if (cx < 320 && cy >= 240) pos_label = "leftdown";
                    else pos_label = "rightdown";

                    // 统计金额
                    int price = PRICES[det_result->cls_id];
                    total_price += price;

                    // 标签名
                    QString label_name = QString::fromUtf8(obj_coco_cls_to_name(det_result->cls_id));

                    // 保存到列表
                    DetectResultInfo info;
                    info.price = price;
                    info.pos_label = pos_label;
                    info.label_name = label_name;
                    info.score = det_result->prop;
                    detectResults.append(info);

                    // 显示类别、置信度、价格、位置
                    sprintf(text, "%s %dyuan %s %.1f%%", obj_coco_cls_to_name(det_result->cls_id), price, pos_label.toUtf8().data(), det_result->prop * 100);
                    draw_text(&src_image, text, x1, y1 - 20, COLOR_RED, 10);



                  // 新增：只取第一个目标标签并发射信号
                  if (i == 0) {
                      QString label = QString::fromUtf8(obj_coco_cls_to_name(det_result->cls_id));
                       emit detectLabel(label, pos_label); // 传递区域
                  }
                }

                // 显示总金额
                char total_text[64];
                sprintf(total_text, "totalprice: %d", total_price);
                draw_text(&src_image, total_text, 10, 30, COLOR_GREEN, 20);

                // 新增：发送信号通知检测结果
                emit detectResultsReady(detectResults, total_price);
            }
            QImage qimg = QImage(src_image.virt_addr,640,480,QImage::Format_RGB888);
            //            qimg = qimg.scaled(320,320);
            showImg(qimg);

        }
        cats.release();
        goto out;
    }else if(videoStatus==1){

    }
out:
    obj_deinit_post_process();//清空识别物品类别变量
    ret = release_yolo_model(&rknn_app_ctx);//清空rknn应用上下文内容结构对象rknn_app_context_t
    if (ret != 0){
        printf("release_yolov5_model fail! ret=%d\n", ret);
    }
}
