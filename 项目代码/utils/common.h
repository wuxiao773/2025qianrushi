#ifndef _RKNN_MODEL_ZOO_COMMON_H_
#define _RKNN_MODEL_ZOO_COMMON_H_

/**
 * @brief Image pixel format
 * 
 */
typedef enum {
    IMAGE_FORMAT_GRAY8,
    IMAGE_FORMAT_RGB888,
    IMAGE_FORMAT_RGBA8888,
    IMAGE_FORMAT_YUV420SP_NV21,
    IMAGE_FORMAT_YUV420SP_NV12,
} image_format_t;

/**
 * @brief Image buffer
 * 
 */
typedef struct {
    int width;
    int height;
    int width_stride;
    int height_stride;
    image_format_t format;
    unsigned char* virt_addr;//图片数据
    int size;
    int fd;
} image_buffer_t;

/**
 * @brief Image rectangle
 * 简单图像巨型，左边顶点，右下底点
 */
typedef struct {
    int left;  //左
    int top;   //顶
    int right; //右
    int bottom;//低
} image_rect_t;

#endif //_RKNN_MODEL_ZOO_COMMON_H_
