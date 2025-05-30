#ifndef EXPANDABLEBUTTON_H
#define EXPANDABLEBUTTON_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QLabel>
#include <QIcon>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>
#include "QList"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class ExpandableButton : public QWidget
{
    Q_OBJECT
public:
    explicit ExpandableButton(QWidget *parent = nullptr);
    void addSubButton(QPushButton* button); // 添加子按钮的接口
signals:
    void popSelectedBtn(QString str);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private slots:
    void togglePopup();
    void onSubButtonClicked();
private:
    void updatePopupPosition();
    void setupPopupPanel();
    QPushButton *mainButton;
    QPushButton *currentBtn;
    bool flag;
    QWidget *popupPanel;
    QGridLayout *popupLayout;
    bool popupVisible;

};

#endif // EXPANDABLEBUTTON_H
