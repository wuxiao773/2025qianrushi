#include "multilightsensor.h"
#include <QTimer>
#include <gpiod.h>
#include <unistd.h>


// HX711读取函数（你需要实现并调试）
long readHX711(struct gpiod_line* dout, struct gpiod_line* sck) {
    if (!dout || !sck) return 0;
    long count = 0;
    int i;
    // 等待DOUT变低
    while (gpiod_line_get_value(dout) == 1);
    for (i = 0; i < 24; i++) {
        gpiod_line_set_value(sck, 1);
        usleep(1);
        count = count << 1;
        gpiod_line_set_value(sck, 0);
        usleep(1);
        if (gpiod_line_get_value(dout))
            count++;
    }
    // 第25个脉冲
    gpiod_line_set_value(sck, 1);
    usleep(1);
    gpiod_line_set_value(sck, 0);
    usleep(1);

    count ^= 0x800000; // 补码转换
    return count;
}

MultiLightSensor::MultiLightSensor(QObject *parent)
    : QObject(parent), m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &MultiLightSensor::updateValues);

    // 1. 打开GPIO芯片
    chip = gpiod_chip_open_by_name("gpiochip0"); // 视实际情况调整
    if (!chip) {
            qWarning("Failed to open gpiochip0!");
            return;
        }
    // 2. 分配每个通道的DOUT和SCK引脚编号（请根据实际接线修改）
    int dout_pins[4] = {28, 19, 21, 25}; // 示例：4个DOUT引脚编号
    int sck_pins[4]  = {29, 20, 22, 26}; // 示例：4个SCK引脚编号

    // 3. 获取每个通道的句柄并设置方向
    for (int i = 0; i < 4; ++i) {
        dout[i] = gpiod_chip_get_line(chip, dout_pins[i]);
        sck[i]  = gpiod_chip_get_line(chip,  sck_pins[i]);
        //gpiod_line_request_input(dout[i], "hx711");
       // gpiod_line_request_output(sck[i], "hx711", 0);
        if (!dout[i] || !sck[i]) {
                   dout[i] = nullptr;
                   sck[i] = nullptr;
                   qWarning("GPIO line get failed at index %d", i);
                   continue;
               }
               if (gpiod_line_request_input(dout[i], "hx711") < 0) {
                   qWarning("Failed to request input for dout[%d]", i);
                   dout[i] = nullptr;
               }
               if (gpiod_line_request_output(sck[i], "hx711", 0) < 0) {
                   qWarning("Failed to request output for sck[%d]", i);
                   sck[i] = nullptr;
               }
    }
}

MultiLightSensor::~MultiLightSensor() {
    // 释放gpiod资源
    for (int i = 0; i < 4; ++i) {
        if (dout[i]) gpiod_line_release(dout[i]);
        if (sck[i])  gpiod_line_release(sck[i]);
    }
    if (chip) gpiod_chip_close(chip);
}

void MultiLightSensor::start()
{
    m_timer->start(1000); // 每秒采集一次
}

void MultiLightSensor::stop()
{
    m_timer->stop();
}

void MultiLightSensor::updateValues()
{
    int v[4] = {0};
    for (int i = 0; i < 4; ++i) {
        if (dout[i] && sck[i])
                   v[i] = static_cast<int>(readHX711(dout[i], sck[i]));
               else
                   v[i] = 0;
    }
    emit valuesChanged(v[0], v[1], v[2], v[3]);
}
