#include "detectlabel.h"

DetectLabel::DetectLabel(QObject *parent)
    : QObject(parent)
{
}

void DetectLabel::setLabel(const QString &label)
{
    if (m_label != label) {
        m_label = label;
        emit labelChanged(m_label);
    }
}
