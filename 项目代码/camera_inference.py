import cv2
import numpy as np
from rknnlite.api import RKNNLite
import argparse
import os
from yolov5_inference import post_process
from coco_utils import COCO_test_helper

COCO_CLASSES = [
    'Roast_duck', 'Sweet_pork', 'Mapo', 'YuXiang_rose', 'Lung', 
    'Twice_cook', 'Gongbao_chicken', 'Saliva_chicken', 'Xiaolongbao', 'Shizi_head',
    'Dongpo_port', 'Xihu', 'Fo', 'fish', 'suan_fish', 'la_fish'
]
# 为每个菜品添加价格（单位：元）
COCO_PRICES = [
    88, 38, 28, 32, 42,
    36, 35, 30, 25, 40,
    58, 48, 168, 46, 39, 45
]

def main():
    parser = argparse.ArgumentParser(description='Real-time inference using RKNN model')
    parser.add_argument('--model_path', type=str, default='yolov5s_rk3588.rknn', help='Path to RKNN model file')
    parser.add_argument('--obj_thresh', type=float, default=0.50, help='Object confidence threshold')
    parser.add_argument('--nms_thresh', type=float, default=0.65, help='NMS threshold')
    parser.add_argument('--img_size', type=int, nargs=2, default=[640, 640], help='Image size (width height)')
    args = parser.parse_args()

    global OBJ_THRESH, NMS_THRESH, IMG_SIZE
    OBJ_THRESH = args.obj_thresh
    NMS_THRESH = args.nms_thresh
    IMG_SIZE = tuple(args.img_size)

    ANCHORS = [
        [[10.0, 13.0], [16.0, 30.0], [33.0, 23.0]],
        [[30.0, 61.0], [62.0, 45.0], [59.0, 119.0]],
        [[116.0, 90.0], [156.0, 198.0], [373.0, 326.0]]
    ]

    rknn = RKNNLite(verbose=0)
    print('Loading RKNN model...')
    ret = rknn.load_rknn(args.model_path)
    if ret != 0:
        print('Failed to load model!')
        exit(ret)
    print('Initializing runtime environment...')
    ret = rknn.init_runtime()
    if ret != 0:
        print('Failed to initialize runtime environment!')
        exit(ret)

    cap = cv2.VideoCapture(11)
    if not cap.isOpened():
        print("Error: Could not open video.")
        exit(1)

    co_helper = COCO_test_helper(enable_letter_box=True)
    
    cv2.namedWindow("Cam",cv2.WINDOW_NORMAL)
    cv2.resizeWindow("Cam",1280,720)
    
    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Could not read frame.")
            break

        img = co_helper.letter_box(im=frame.copy(), new_shape=(IMG_SIZE[1], IMG_SIZE[0]), pad_color=(0, 0, 0))
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        img = np.expand_dims(img, axis=0)

        outputs = rknn.inference(inputs=[img], data_format=['nhwc'])
        if outputs is None:
            print("Inference failed, output is None!")
            break

        boxes, classes, scores = post_process(outputs, ANCHORS)

        # 统计总金额
        total_price = 0

        if boxes is not None:
            boxes = co_helper.get_real_box(boxes)
            h, w = frame.shape[:2]
            for box, score, cl in zip(boxes, scores, classes):
                top, left, right, bottom = [int(_b) for _b in box]
                # 计算中心点
                cx = (top + right) // 2
                cy = (left + bottom) // 2
                # 判断位置
                if cx < w // 2 and cy < h // 2:
                    pos_label = "左上"
                elif cx >= w // 2 and cy < h // 2:
                    pos_label = "右上"
                elif cx < w // 2 and cy >= h // 2:
                    pos_label = "左下"
                else:
                    pos_label = "右下"
                cv2.rectangle(frame, (top, left), (right, bottom), (255, 0, 255), 6)
                class_name = COCO_CLASSES[cl] if cl < len(COCO_CLASSES) else str(cl)
                price = COCO_PRICES[cl] if cl < len(COCO_PRICES) else 0
                total_price += price
                text = f'{class_name} {price} {pos_label} Believe:{score:.2f}'
                font = cv2.FONT_HERSHEY_SIMPLEX
                try:
                    from PIL import ImageFont, ImageDraw, Image
                    pil_img = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
                    draw = ImageDraw.Draw(pil_img)
                    font_pil = ImageFont.truetype("msyh.ttc", 40)
                    draw.text((top, left - 40), text, font=font_pil, fill=(255, 0, 255))
                    frame = cv2.cvtColor(np.array(pil_img), cv2.COLOR_RGB2BGR)
                except:
                    cv2.putText(frame, text, (top, left - 10), font, 2.0, (255, 0, 255), 2)

        # 在画面左上角显示总金额
        total_text = f'total_price: {total_price}'
        try:
            from PIL import ImageFont, ImageDraw, Image
            pil_img = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
            draw = ImageDraw.Draw(pil_img)
            font_pil = ImageFont.truetype("msyh.ttc", 40)
            draw.text((20, 20), total_text, font=font_pil, fill=(0, 255, 0))
            frame = cv2.cvtColor(np.array(pil_img), cv2.COLOR_RGB2BGR)
        except:
            cv2.putText(frame, total_text, (20, 40), cv2.FONT_HERSHEY_SIMPLEX, 1.8, (0, 255, 0), 5)

        cv2.imshow('Cam', frame)
        
        key = cv2.waitKey(1) & 0xFF
        if key ==ord('q'):
            break
        elif key == ord('+'):
            cv2.resizeWindow("Cam",int(cv2.getWindowImageRect("Cam")[2]*1.1),
			int(cv2.getWindowImageRect("Cam")[3]*1.1))
        elif key == ord('-'):
            cv2.resizeWindow("Cam",int(cv2.getWindowImageRect("Cam")[2]*0.9),
			int(cv2.getWindowImageRect("Cam")[3]*0.9))
    cap.release()
    cv2.destroyAllWindows()
    rknn.release()

if __name__ == '__main__':
    main()
