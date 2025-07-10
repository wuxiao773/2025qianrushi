#ifndef _RKNN_YOLOV5_DEMO_POSTPROCESS_H_
#define _RKNN_YOLOV5_DEMO_POSTPROCESS_H_

#include <stdint.h>
#include <vector>
#include "3rdparty/rknpu2/include/rknn_api.h"
#include "utils/common.h"
#include "utils/image_utils.h"
#include "yolo.h"
#define OBJ_NAME_MAX_SIZE 64
#define OBJ_NUMB_MAX_SIZE 128
#define OBJ_CLASS_NUM 16
#define NMS_THRESH 0.65
#define BOX_THRESH 0.65
#define PROP_BOX_SIZE (5 + OBJ_CLASS_NUM)

// class rknn_app_context_t;

typedef struct {
    image_rect_t box;
    float prop;
    int cls_id;
} object_detect_result;

typedef struct {
    int id;
    int count;
    object_detect_result results[OBJ_NUMB_MAX_SIZE];
} object_detect_result_list;

int obj_init_post_process();
void obj_deinit_post_process();
char *obj_coco_cls_to_name(int cls_id);
int obj_post_process(rknn_app_context_t *app_ctx, void *outputs, letterbox_t *letter_box, float conf_threshold, float nms_threshold, object_detect_result_list *od_results);
void obj_deinit_post_process();
int v8obj_init_post_process();
void v8obj_deinit_post_process();
char *v8obj_coco_cls_to_name(int cls_id);
int v8obj_post_process(rknn_app_context_t *app_ctx, rknn_output *outputs, letterbox_t *letter_box, float conf_threshold, float nms_threshold, object_detect_result_list *od_results);

#endif //_RKNN_YOLOV5_DEMO_POSTPROCESS_H_
