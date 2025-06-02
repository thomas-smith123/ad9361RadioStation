#include "meter.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMeter w;
    w.show();
    return a.exec();
}
