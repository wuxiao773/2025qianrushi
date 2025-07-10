QT       += core gui
QT += core network
QT += multimedia
QT += multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main/detectlabel.cpp \
    src/main/main.cpp \
    src/main/mainwindow.cpp \
    src/aichat/aichat.cpp \
    src/main/multilightsensor.cpp \
    src/main/tablemanager.cpp \
    src/yolo_detect/objpostprocess.cc \
    src/yolo_detect/yolo.cc \
    src/yolo_detect/objectdetect.cpp \
    src/yolo_seg/yoloseg.cpp \
    src/yolo_seg/yolo_seg.cpp \
    src/yolo_seg/postprocess.cpp \
#    src/yolo_obb/yoloobbdetect.cpp \
    utils/image_utils.c \
    utils/file_utils.c \
    utils/image_drawing.c \
    src/community/Community.cpp

HEADERS += \
    src/main/detectlabel.h \
    src/main/mainwindow.h \
    src/aichat/aichat.h \
    src/main/multilightsensor.h \
    src/main/tablemanager.h \
    src/yolo_detect/objpostprocess.h \
    src/yolo_detect/yolo.h \
    src/yolo_detect/objectdetect.h \
    src/yolo_seg/yoloseg.h \
    src/yolo_seg/postprocess.h \
    src/yolo_seg/yolo_seg.h \
#    src/yolo_obb/yoloobbdetect.h \
    utils/image_utils.h \
    utils/file_utils.h \
    utils/image_drawing.h \
    src/community/Community.h



FORMS += \
    src/main/mainwindow.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



unix:!macx: LIBS += -L$$PWD/./ -lrkllmrt

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.


INCLUDEPATH += /usr/include/opencv4/


INCLUDEPATH += ./include/*.h \
               ./include/modules/*.h \
               ./include/utils/*.h

LIBS += -lpthread -ldl \
         -lm -ldl \
        -lopencv_videoio -ldl \
        -lopencv_video -ldl \
        -lopencv_core -ldl \
        -l opencv_highgui -ldl \
        -lopencv_photo -ldl \
        -lopencv_stitching -ldl \
        -lopencv_shape -ldl \
        -lopencv_flann -ldl \
        -lopencv_features2d -ldl \
        -lopencv_calib3d -ldl \
        -lopencv_ml -ldl \
        -lopencv_dnn -ldl \
        -lopencv_imgcodecs -ldl \
        -lopencv_imgproc -ldl \
        -lopencv_objdetect -ldl \
        -lrknnrt  \
        -lrga

LIBS += -lgpiod

INCLUDEPATH += $$PWD/3rdparty/quazip
DEPENDPATH += $$PWD/3rdparty/quazip
unix:!macx: LIBS += -L$$PWD/3rdparty/jpeg_turbo/Linux/aarch64/ -lturbojpeg
INCLUDEPATH += $$PWD/3rdparty/jpeg_turbo/Linux/aarch64
DEPENDPATH += $$PWD/3rdparty/jpeg_turbo/Linux/aarch64

unix:!macx: PRE_TARGETDEPS += $$PWD/3rdparty/jpeg_turbo/Linux/aarch64/libturbojpeg.a

RESOURCES += \
    res.qrc
