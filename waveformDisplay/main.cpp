#include "waveformdisplay.h"
#include "QVBoxLayout"
#include <QApplication>
#include "QPushButton"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 创建主窗口
    QWidget window;
    window.setWindowTitle("Expandable Button Demo");
    window.resize(600, 400);
    window.setStyleSheet("background-color: #f0f2f5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&window);
    // mainLayout->setAlignment(Qt::AlignCenter);


    waveformdisplay *btn = new waveformdisplay;
    mainLayout->addWidget(btn);
    // 添加子按钮

    window.show();
    // return app.exec();
    return a.exec();
}
