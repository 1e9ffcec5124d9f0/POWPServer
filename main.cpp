#include <QtCore/QCoreApplication>
#include"tools.h"
#include"NetManager.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    loadSetting();
    NetManager temp;
    return a.exec();
}
