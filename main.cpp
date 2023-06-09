//#define TEST_APP

#ifdef TEST_APP
#include <QTest>
#include "tests.h"
#else
#include <QApplication>
#include "mainwindow.h"
#endif

int main(int argc, char *argv[])
{
#ifdef TEST_APP
    return QTest::qExec(new LogicTest(), argc, argv);
#else
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
#endif
}
