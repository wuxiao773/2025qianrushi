#ifndef MULTILIGHTSENSOR_H
#define MULTILIGHTSENSOR_H

#include <QObject>
#include <QTimer>
#include <gpiod.h> // 新增

class MultiLightSensor : public QObject
{
    Q_OBJECT
public:
    explicit MultiLightSensor(QObject *parent = nullptr);
    ~MultiLightSensor();
    void start();
    void stop();

signals:
    void valuesChanged(int v1, int v2, int v3, int v4);

private:
    QTimer *m_timer;
    void updateValues();

    // 新增HX711相关成员
    struct gpiod_chip* chip = nullptr;
    struct gpiod_line* dout[4] = {nullptr};
    struct gpiod_line* sck[4]  = {nullptr};
};

#endif // MULTILIGHTSENSOR_H
