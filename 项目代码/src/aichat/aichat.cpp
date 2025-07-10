#include "aichat.h"
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
#define DEEPSEEK_API_URL "https://api.deepseek.com/chat/completions"
#define DEEPSEEK_API_KEY "sk-73a0c5bb0dea4407bc039a14dc38a2ac" // TODO: 替换为你的API KEY

AiChat::AiChat(QObject *parent)
    : QThread{parent}, networkManager(new QNetworkAccessManager(this))
{
}

void AiChat::AiInit()
{
    emit setParValue(100); // 直接完成
}
void AiChat::aiChat(QString chat)
{
    emit sendChatInfo("robot: ", 0);

    QNetworkRequest request(QUrl(DEEPSEEK_API_URL));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " DEEPSEEK_API_KEY);

    // 构造多轮对话 messages
    QJsonArray messagesArr;
    if (messageHistory.isEmpty()) {
        // 首次对话，添加 system prompt
        QJsonObject systemObj;
        systemObj["role"] = "system";
        systemObj["content"] = "You are a helpful assistant.";
        messagesArr.append(systemObj);
    }
    // 添加历史消息
    for (const QJsonObject &msg : messageHistory) {
        messagesArr.append(msg);
    }
    // 添加当前用户输入
    QJsonObject userObj;
    userObj["role"] = "user";
    userObj["content"] = chat;
    messagesArr.append(userObj);
    QJsonObject bodyObj;
    bodyObj["model"] = "deepseek-chat";
    bodyObj["messages"] = messagesArr;
    bodyObj["temperature"] = 0.8;
    bodyObj["top_p"] = 0.95;
    bodyObj["max_tokens"] = 2048;
    bodyObj["stream"] = false;

    QJsonDocument doc(bodyObj);
    QByteArray postData = doc.toJson();

    QNetworkReply *reply = networkManager->post(request, postData);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument respDoc = QJsonDocument::fromJson(response);
        if (respDoc.isObject()) {
            QJsonObject obj = respDoc.object();
            if (obj.contains("choices")) {
                QJsonArray choices = obj["choices"].toArray();
                if (!choices.isEmpty()) {
                    QJsonObject choice = choices[0].toObject();
                    QJsonObject message = choice["message"].toObject();
                    QString content = message["content"].toString();
                    emit sendChatInfo(content, 2);
                   // 保存本轮 user 和 assistant 消息到历史
                   QJsonObject userMsg;
                   userMsg["role"] = "user";
                   userMsg["content"] = chat;
                   messageHistory.append(userMsg);
                   QJsonObject assistantMsg;
                   assistantMsg["role"] = "assistant";
                   assistantMsg["content"] = content;
                   messageHistory.append(assistantMsg);

                }
            }
        }
    } else {
        emit sendChatInfo("[DeepSeek API error] " + reply->errorString(), 1);
    }
    reply->deleteLater();
}

void AiChat::run()
{
    aiChat(chat);
}
