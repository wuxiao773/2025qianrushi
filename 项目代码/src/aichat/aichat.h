#ifndef AICHAT_H
#define AICHAT_H

#include <QObject>
#include <string.h>
#include <unistd.h>
#include <string>
#include "rkllm.h"
#include <fstream>
#include <iostream>
#include <csignal>
#include <vector>
#include <QThread>
#include <QtDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QJsonObject>
#include <QList>
class AiChat : public QThread
{
    Q_OBJECT
public:
    explicit AiChat(QObject *parent = nullptr);
    void AiInit();
    static void callback(RKLLMResult *result, void *userdata, LLMCallState state);
    void  aiChat(QString chat);
    void hardCallback(QString,int status);
    void run();
    QString chat;
signals:
   QString sendChatInfo(QString,int status);
   void setParValue(int );

private:
    QNetworkAccessManager *networkManager;
    QList<QJsonObject> messageHistory;
};
 extern AiChat *acht;
#endif // AICHAT_H
