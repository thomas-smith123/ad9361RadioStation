#include "ExpandableButton.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 创建主窗口
    QWidget window;
    window.setWindowTitle("Expandable Button Demo");
    window.resize(600, 400);
    window.setStyleSheet("background-color: #f0f2f5;");

    QVBoxLayout *mainLayout = new QVBoxLayout(&window);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 标题
    QLabel *title = new QLabel("Expandable Button Control");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #333; margin-bottom: 30px;");
    title->setAlignment(Qt::AlignCenter);

    ExpandableButton *btn = new ExpandableButton;
    mainLayout->addWidget(btn);
    // 添加子按钮
    for(int i = 1; i <= 9; ++i) {
        QPushButton *subBtn = new QPushButton(QString::number(i));
        subBtn->setFixedSize(30, 30);
        btn->addSubButton(subBtn);
    }

    window.show();
    // return app.exec();
    return a.exec();
}
