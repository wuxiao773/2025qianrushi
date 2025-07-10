#ifndef YOLOSEG_H
#define YOLOSEG_H
#include <QImage>
#include <QThread>
class YoloSeg : public QThread
{
    Q_OBJECT
public:
    explicit YoloSeg(QObject *parent = nullptr);

signals:
    void showImg(QImage img);

protected:
    void run();
};

#endif // YOLOSEG_H
