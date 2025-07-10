#ifndef DETECTLABEL_H
#define DETECTLABEL_H

#include <QObject>
#include <QString>

class DetectLabel : public QObject
{
    Q_OBJECT
public:
    explicit DetectLabel(QObject *parent = nullptr);
    void setLabel(const QString &label);

signals:
    void labelChanged(const QString &label);

private:
    QString m_label;
};

#endif // DETECTLABEL_H
