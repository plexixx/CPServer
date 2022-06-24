#include "cpserver.h"
#include <QApplication>
#include "head.h"
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CPServer c;
    TcpServer tsr;
    //c.show();

    return a.exec();
}
