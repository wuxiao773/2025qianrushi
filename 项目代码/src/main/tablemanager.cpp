#include "tablemanager.h"

TableManager::TableManager(QTableWidget *tableWidget, QObject *parent)
    : QObject(parent), m_tableWidget(tableWidget)
{
    if (m_tableWidget) {
        m_tableWidget->setColumnCount(2);
        m_tableWidget->setRowCount(4); // 固定4行
        QStringList headers;
        headers << "菜名" << "质量";
        m_tableWidget->setHorizontalHeaderLabels(headers);
        m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void TableManager::setRowData(int row, const QString &label, int lightValue)
{
    if (!m_tableWidget) return;
    if (row >= 0 && row < 4) { // 只允许0~3行
        m_tableWidget->setItem(row, 0, new QTableWidgetItem(label));
        m_tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(lightValue)));
    }
}

void TableManager::addRow(const QString &label, int lightValue)
{
    if (!m_tableWidget) return;
    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);
    setRowData(row, label, lightValue);
}

void TableManager::clear()
{
    if (m_tableWidget) {
        m_tableWidget->setRowCount(0);
    }
}
