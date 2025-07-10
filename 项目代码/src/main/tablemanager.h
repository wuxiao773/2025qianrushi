#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QObject>
#include <QTableWidget>

class TableManager : public QObject
{
    Q_OBJECT
public:
    explicit TableManager(QTableWidget *tableWidget, QObject *parent = nullptr);

    // 设置表格中的标签和光敏值
    void setRowData(int row, const QString &label, int lightValue);
    // 添加一行数据
    void addRow(const QString &label, int lightValue);
    // 清空表格
    void clear();

private:
    QTableWidget *m_tableWidget;
};

#endif // TABLEMANAGER_H
