#include "ExpandableButton.h"
#include <QEvent>
#include "QDebug"

// #include "ui_widget.h"
ExpandableButton::ExpandableButton(QWidget *parent)
    : QWidget(parent), popupVisible(false)
{
    // 主按钮设置
    mainButton = new QPushButton(this);
    flag = false;
    mainButton->setText("Select");
    mainButton->installEventFilter(this);
    connect(mainButton, &QPushButton::clicked, this, &ExpandableButton::togglePopup);
    // 弹出面板设置
    setupPopupPanel();
    // 初始布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(mainButton);
    layout->setContentsMargins(0, 0, 0, 0);
}
void ExpandableButton::setupPopupPanel()
{
    popupPanel = new QWidget(nullptr, Qt::Popup);
    popupPanel->setStyleSheet("QWidget { border: 1px solid gray; }");
    popupLayout = new QGridLayout(popupPanel);
    popupLayout->setSpacing(2);
    popupLayout->setContentsMargins(5, 5, 5, 5);
    popupPanel->hide();
}
void ExpandableButton::addSubButton(QPushButton *button)
{
    button->setParent(popupPanel);
    const int count = popupLayout->count();
    popupLayout->addWidget(button, count/3, count%3);
    connect(button, &QPushButton::clicked, this, &ExpandableButton::onSubButtonClicked);
}
bool ExpandableButton::eventFilter(QObject *watched, QEvent *event)
{
    // 鼠标悬停触发
    if (watched == mainButton && event->type() == QEvent::Enter) {
        if (!popupVisible) {
            togglePopup();
        }
    }
    return QWidget::eventFilter(watched, event);
}
void ExpandableButton::togglePopup()
{
    popupVisible = !popupVisible;
    if (popupVisible) {
        updatePopupPosition();
        popupPanel->show();
    } else {
        popupPanel->hide();
    }
}
void ExpandableButton::updatePopupPosition()
{
    QPoint globalPos = mainButton->mapToGlobal(QPoint(0, mainButton->height()));
    popupPanel->move(globalPos);
    popupPanel->adjustSize();
}
void ExpandableButton::onSubButtonClicked()
{
    if(flag)
        currentBtn->setStyleSheet(R"(
                QPushButton {
                    background-color: #f5f7fa;
                    border: 1px solid #e1e5eb;
                    border-radius: 5px;
                    padding: 5px;
                    font-size: 12px;
                }
            )");
    currentBtn = qobject_cast<QPushButton*>(sender());

    if (currentBtn) {
        flag = true;
        currentBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #f5f7fa;
                border: 1px solid #e1e5eb;
                border-radius: 15px;
                padding: 5px;
                font-size: 12px;
            }
        )");
        mainButton->setText(currentBtn->text());
        mainButton->setIcon(currentBtn->icon());
        popSelectedBtn(currentBtn->text());
        qDebug()<<currentBtn->text();

    }
    popupVisible = false;
    popupPanel->hide();
    // add signal here
}
