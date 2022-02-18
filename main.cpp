#include<QtWidgets/qapplication.h>
#include"tools.h"
#include"MainWindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow b;
    b.show();
    return a.exec();
}
