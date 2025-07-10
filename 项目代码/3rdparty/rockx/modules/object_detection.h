/****************************************************************************
 *
 *    Copyright (c) 2018 - 2022 by Rockchip Corp.  All rights reserved.
 *
 *    The material in this file is confidential and contains trade secrets
 *    of Rockchip Corporation. This is proprietary information owned by
 *    Rockchip Corporation. No part of this work may be disclosed,
 *    reproduced, copied, transmitted, or used in any way for any purpose,
 *    without the express written permission of Rockchip Corporation.
 *
 *****************************************************************************/

#ifndef _ROCKX_OBJECT_DETECTION_H
#define _ROCKX_OBJECT_DETECTION_H

#include "3rdparty/rockx/rockx_type.h"

#ifdef __cplusplus
extern "C" {
#endif

extern rockx_module_t ROCKX_MODULE_MOVE_DETECTOR;
extern rockx_module_t ROCKX_MODULE_OBJECT_DETECTION;     ///< Object detection (result class 0:person; 1:face, 2:vehicle; 3: non-vehicle)
extern rockx_module_t ROCKX_MODULE_PERSON_DETECTION;     ///< Person detection
extern rockx_module_t ROCKX_MODULE_HEAD_DETECTION;       ///< Head detection

/**
 * Object Detection
 * @param handle [in] Handle of a created module(created by @ref rockx_create)
 * @param in_img [in] Input image
 * @param object_array [out] Detection Result
 * @return @ref rockx_ret_t
 */
rockx_ret_t rockx_object_detect(rockx_handle_t handle, rockx_image_t* in_img, rockx_object_array_t* object_array);


/**
 * MOVE Detector
 * @param handle [in] Handle of a created ROCKX_MODULE_MOVE_DETECTOR module(created by @ref rockx_create)
 * @param in_img1 [in] Input image1
 * @param in_img2 [in] Input image2
 * @param location[20] Detection Result (percent), the oeder is [xmin1,xmax1,ymin1,ymax1,xmin2,xmax2,ymin2,ymax2,...]
 * @return @ref rockx_ret_t
 */
rockx_ret_t rockx_move_detector(rockx_handle_t handle,
                        rockx_image_t *in_img1,rockx_image_t *in_img2,float location[20]);

/**
 * Head Detection
 * @param handle [in] Handle of a created ROCKX_MODULE_HEAD_DETECTION module(created by @ref rockx_create)
 * @param in_img [in] Input image
 * @param object_array [out] Detection Result
 * @return @ref rockx_ret_t
 */
rockx_ret_t rockx_head_detect(rockx_handle_t handle, rockx_image_t* in_img, rockx_object_array_t* object_array);

/**
 * Person Detection
 * @param handle [in] Handle of a created ROCKX_MODULE_BODY_DETECTION module(created by @ref rockx_create)
 * @param in_img [in] Input image
 * @param object_array [out] Detection Result
 * @return @ref rockx_ret_t
 */
rockx_ret_t rockx_person_detect(rockx_handle_t handle, rockx_image_t* in_img, rockx_object_array_t* object_array);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // _ROCKX_OBJECT_DETECTION_H
