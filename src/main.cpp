
#include <QApplication>
#include "MainWindow.h"

#ifdef Q_OS_WINDOWS
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gm::MainWindow w;
    w.show();
    return a.exec();
}
