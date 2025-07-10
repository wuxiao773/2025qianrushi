#ifndef OBJECTDETECT_H
#define OBJECTDETECT_H

#include <QThread>
#include <QImage>
#include <QDebug>
struct DetectResultInfo {
int price;
QString pos_label;
QString label_name;
float score;
};

class ObjectDetect : public QThread
{
    Q_OBJECT
public:
    explicit ObjectDetect(QObject *parent = nullptr);
protected:
    void run();
signals:
    void showImg(QImage img);
     void detectLabel(const QString &label, const QString &pos_label); // 传递标签和区域
    void detectResultsReady(const QList<DetectResultInfo>& results, int total_price);
};

#endif // OBJECTDETECT_H
