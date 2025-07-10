#include "yoloseg.h"
#include "src/community/Community.h"
#include "opencv2/opencv.hpp"
#include "src/yolo_seg/yolo_seg.h"
#include "3rdparty/stb_image/timer.h"
#include "utils/image_drawing.h"
#include <QMap>
#include <queue>
#include <vector>
#include <QCameraInfo>
using namespace cv;

YoloSeg::YoloSeg(QObject *parent)
    : QThread{parent}
{

}


/**
 * 释放rknn：包括类别信息对象释放、rknn模型对象释放
 * 示例：releaseRknn(rknn_app_ctx)
 * @brief releaseRknn
 * @param rknn_app_ctx
 */
void releaseRknn(rknn_app_context_t rknn_app_ctx){
    deinit_post_process();
    int ret = release_yolo_seg_model(&rknn_app_ctx);
    if (ret != 0){
        printf("release_yolov5_seg_model fail! ret=%d\n", ret);
    }
}
unsigned char class_colors[][3] = {//类颜色
    {255, 56, 56},   // 'FF3838'
    {255, 157, 151}, // 'FF9D97'
    {255, 112, 31},  // 'FF701F'
    {255, 178, 29},  // 'FFB21D'
    {207, 210, 49},  // 'CFD231'
    {72, 249, 10},   // '48F90A'
    {146, 204, 23},  // '92CC17'
    {61, 219, 134},  // '3DDB86'
    {26, 147, 52},   // '1A9334'
    {0, 212, 187},   // '00D4BB'
    {44, 153, 168},  // '2C99A8'
    {0, 194, 255},   // '00C2FF'
    {52, 69, 147},   // '344593'
    {100, 115, 255}, // '6473FF'
    {0, 24, 236},    // '0018EC'
    {132, 56, 255},  // '8438FF'
    {82, 0, 133},    // '520085'
    {203, 56, 255},  // 'CB38FF'
    {255, 149, 200}, // 'FF95C8'
    {255, 55, 199}   // 'FF37C7'
};

/**
 * image_buffer_t  src_image= dram(Mat fram,rknn_app_context_t rknn_app_ctx);
 * @brief dram
 * @param fram
 * @param rknn_app_ctx
 * @return
 */
image_buffer_t dram(Mat fram,rknn_app_context_t rknn_app_ctx){
    int ret;
    cv::Mat img;
    cv::cvtColor(fram, img, cv::COLOR_BGR2RGB);//Mat limg;//cv::resize(fram,limg,Size(640,640));
    object_detect_result_list od_results;
    image_buffer_t src_image;
    memset(&src_image, 0, sizeof(image_buffer_t));
    src_image.width = img.cols;
    src_image.height = img.rows;
    src_image.format = IMAGE_FORMAT_RGB888;
    src_image.size = img.rows*img.cols*3;
    unsigned char * data = (unsigned char *)img.data;
    src_image.virt_addr = data;
    //1、od_results:识别后返回的结果；2、segStatus=0表示yolov5;1表示yolov8;
    ret = inference_yolo_seg_model(&rknn_app_ctx, &src_image, &od_results,yoloStatus);
    if(ret != 0){
        printf("init_yolov5_seg_model fail! ret=%d\n", ret);
        releaseRknn(rknn_app_ctx);
    }
    if (od_results.count >= 1){ //画实例分割draw mask
        int width = src_image.width;
        int height = src_image.height;
        char *ori_img = (char *)src_image.virt_addr;
        int cls_id = od_results.results[0].cls_id;
        uint8_t *seg_mask = od_results.results_seg[0].seg_mask;
        float alpha = 0.5f;     //opacity
        for (int j = 0; j < height; j++){   //高度
            for (int k = 0; k < width; k++){//宽度
                int pixel_offset = 3 * (j * width + k);//像素偏移量
                if (seg_mask[j * width + k] != 0){
                    ori_img[pixel_offset+0]=(unsigned char)clamp(class_colors[seg_mask[j*width+k]%20][0]*(1-alpha)+ori_img[pixel_offset+0]*alpha,0,255);//r
                    ori_img[pixel_offset+1]=(unsigned char)clamp(class_colors[seg_mask[j*width+k]%20][1]*(1-alpha)+ori_img[pixel_offset+1]*alpha,0,255);//g
                    ori_img[pixel_offset+2]=(unsigned char)clamp(class_colors[seg_mask[j*width+k]%20][2]*(1-alpha)+ori_img[pixel_offset+2]*alpha,0,255);//b
                }
            }
        }
        free(seg_mask);
    }
    char text[256];//画目标框draw boxes
    for (int i = 0; i < od_results.count; i++){
        object_detect_result *det_result = &(od_results.results[i]);
        printf("%s @ (%d %d %d %d) %.3f\n", coco_cls_to_name(det_result->cls_id),
               det_result->box.left, det_result->box.top,det_result->box.right, det_result->box.bottom,det_result->prop);
        int x1 = det_result->box.left;
        int y1 = det_result->box.top;
        int x2 = det_result->box.right;
        int y2 = det_result->box.bottom;
        draw_rectangle(&src_image, x1, y1, x2 - x1, y2 - y1, COLOR_RED, 3);
        sprintf(text, "%s %.1f%%", coco_cls_to_name(det_result->cls_id), det_result->prop * 100);
        draw_text(&src_image, text, x1, y1 - 16+100, COLOR_BLUE, 10);
    }
    return src_image;
}

/**
 * rknn_app_context_t初始化rknn：包括类别信息加载、rknn模型加载
 * 示例：initRknn()
 * @brief initRknn
 * @return
 */
string initRknn(rknn_app_context_t &rknn_app_ctx){
    string rmsg ="success"; //接收成功信息
    int ret;
    memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t));
    ret = init_post_process();//1、读取文件labelPathH识别物品类别数据，放到全局label里面
    if (ret != 0){
        rmsg="obj_init_post_process fail! ret= "+to_string(ret);
        releaseRknn(rknn_app_ctx);
        return rmsg;
    }
    ret = init_yolo_seg_model(modelPath.toStdString().c_str(), &rknn_app_ctx);
    if (ret != 0){
        rmsg="init_yolo_model fail!ret="+to_string(ret);//printf("init_yolo_model fail! ret=%d \n", ret);
        releaseRknn(rknn_app_ctx);
        return rmsg;
    }
    return rmsg;
}

void YoloSeg::run()
{
    rknn_app_context_t rknn_app_ctx;
    string rmsg=initRknn(rknn_app_ctx);

    if(videoStatus==0){ //摄像头
        qDebug()<<"摄像头打开中请稍等";
        QList<QCameraInfo> list = QCameraInfo::availableCameras();
        if(list.size()==0){
        qDebug()<<"摄像头未检查到摄像头";
        releaseRknn(rknn_app_ctx);
        return;
        }
        QCameraInfo  cameraInfo=QCameraInfo::defaultCamera();
        string cameraPath= cameraInfo.deviceName().toStdString();
        VideoCapture cats(cameraPath);
        cats.set(cv::CAP_PROP_FRAME_WIDTH,640);//设置后无法：无法打开“单”摄像头节点?
        cats.set(cv::CAP_PROP_FRAME_HEIGHT,480);//设置后无法：无法打开“单”摄像头节点?
        Mat fram;
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
            image_buffer_t  src_image= dram(fram,rknn_app_ctx);//IMAGE_FORMAT_RGB888
                   //1、Format_RGB888偏紫;2、Format_BGR888正常偏暗;
            QImage qimg = QImage(src_image.virt_addr,src_image.width,src_image.height,QImage::Format_RGB888);
            emit showImg(qimg);
        }
        cats.release();
    }else if(videoStatus==1){//视频
//        VideoCapture cats(videoPath1.toStdString());
//        cats.set(cv::CAP_PROP_FRAME_WIDTH,640);//设置后无法：无法打开“单”摄像头节点?
//        cats.set(cv::CAP_PROP_FRAME_HEIGHT,480);//设置后无法：无法打开“单”摄像头节点?
//        Mat fram;
//        while(runStatus1){
//            bool fag = cats.read(fram);
//            if(fag==false){
//                qDebug()<<"read is false";
//                break;
//            }
//            if(fram.empty()){
//                printf("fram is null");
//                continue;
//            }
//            image_buffer_t  src_image= dram(fram,rknn_app_ctx);//IMAGE_FORMAT_RGB888
//                   //1、Format_RGB888偏紫;2、Format_BGR888正常偏暗;
//            QImage qimg = QImage(src_image.virt_addr,src_image.width,src_image.height,QImage::Format_RGB888);
//            emit showImg(qimg);
//        }
    }else if(videoStatus==2){//图片
//       image_buffer_t src_image1;
//       memset(&src_image1, 0, sizeof(image_buffer_t));
//       int ret = read_image(videoPath1.toStdString().c_str(), &src_image1);
//       unsigned char* data1 =src_image1.virt_addr;
//       Mat fram =Mat(src_image1.height,src_image1.width,CV_8UC3,data1,0);
//       if(fram.empty()){
//           releaseRknn(rknn_app_ctx);
//         return;
//       }
//        image_buffer_t  src_image= dram(fram,rknn_app_ctx);//IMAGE_FORMAT_RGB888
//               //1、Format_RGB888偏紫;2、Format_BGR888正常偏暗;
//        QImage qimg = QImage(src_image.virt_addr,src_image.width,src_image.height,QImage::Format_BGR888);
//        emit showImg(qimg);

    }
    releaseRknn(rknn_app_ctx);
}
