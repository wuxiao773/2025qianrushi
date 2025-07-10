#include "mainwindow.h"
#include <QProcess>
#include <QApplication>

int countSubstringOccurrences(const std::string& str, const std::string& substr) {
    int count = 0;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos) {
        count++;
        pos += substr.length();
    }
    return count;
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QProcess ps;
     ps.start("ps -e");
     ps.waitForFinished();
     QString rst = ps.readAllStandardOutput();
     std::string substr = "aichat";
     int count = countSubstringOccurrences(rst.toStdString(), substr);//查找fungisys运行进程是否存在
     if(count>1){//存在就不再重新运行
         return 0;
     }
    MainWindow w;
    w.show();
    return a.exec();
}
