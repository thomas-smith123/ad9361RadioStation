#ifndef FFTWITHCURSOR_H
#define FFTWITHCURSOR_H

#include <QWidget>
#include "QPainter"
#include "QMouseEvent"
#include <QObject>
#include <QRectF>
#include <QColor>
#include "QtMath"
#include "qcustomplot/qcustomplot.h"
enum BandType {Dual,USB,LSB};
// 简洁的游标覆盖控件
class fftwithcursor : public QWidget {
    Q_OBJECT
public:
    explicit fftwithcursor(QCustomPlot* plotWidget, QWidget* parent = nullptr, BandType bandtype=USB)
        : QWidget(parent), m_plot(plotWidget), m_dragging(false), m_resizing(false) {
        // 设置透明属性
        setAttribute(Qt::WA_TransparentForMouseEvents, false); // 需要接收鼠标事件
        // setAttribute(Qt::WA_MouseTracking);
        setMouseTracking(true); // 启用鼠标跟踪
        setAttribute(Qt::WA_Hover); // 启用悬停事件
        setStyleSheet("background-color: transparent;");

        // 初始游标位置和大小（相对坐标，相对于绘图区域）
        m_cursorRect = QRectF(0.4, 0, 0.2, 1); // x, y, width, height (0-1)
        update();
        // 初始同步
        syncGeometry();
        this->bandtype = bandtype;
    }

    // 设置游标位置 (相对坐标)
    void setCursorPosition(double x, double y) {
        // 保持游标尺寸不变，只改变位置
        m_cursorRect.moveTo(x, y);
        update();
    }

    // 设置游标大小
    void setCursorSize(double width, double height) {
        m_cursorRect.setSize(QSizeF(width, height));
        m_cursorRect_abs = relativeToAbsolute(cursorRect());
        update();
    }
    double getCF(double LO) //中心频率 返回绝对频率
    {
        QRectF tmp = cursorRect();
        return 0.5*(tmp.left()+tmp.right())*LO;
    }
    double getBW(double LO) //带宽
    {
        QRectF tmp = cursorRect();
        return (tmp.right()-tmp.left())*LO;
    }
    // 获取当前游标位置和大小（相对坐标）
    QRectF cursorRect() const {
        return m_cursorRect;
    }
    // 当频谱图尺寸变化时调用
    void syncGeometry() {
        if (m_plot && m_plot->parentWidget()) {
            // 设置覆盖控件与QCustomPlot相同的位置和大小
            setGeometry(m_plot->geometry());
            raise();  // 确保在最上层
            update();
        }
    }

signals:
    void popCursorPosition(double x, double y);
    void popCursorSize(QRectF a);
public slots:
    void followCursorPosition(double x, double y)
    {
        setCursorPosition(x,y);
    }
    void followCursorSize(QRectF a)
    {
        m_cursorRect = a;
        update();
    }
    void changeBandtype(BandType type){
        bandtype = type;
    }
protected:
    void paintEvent(QPaintEvent*) override {
        // 获取绘图区域（不包括坐标轴）
        QRect plotArea = getPlotArea();
        if (plotArea.isEmpty()) return;
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        // 计算实际像素位置
        QRectF pixelRect = relativeToAbsolute(m_cursorRect);
        QLine line;
        switch (bandtype) {
        case Dual:
            line.setLine((pixelRect.left()+pixelRect.right())/2,pixelRect.bottom(),(pixelRect.left()+pixelRect.right())/2,pixelRect.top());
            break;
        case USB:
            line.setLine(pixelRect.right(),pixelRect.bottom(),pixelRect.right(),pixelRect.top());
            break;
        default:
            line.setLine(pixelRect.left(),pixelRect.bottom(),pixelRect.left(),pixelRect.top());
            break;
        }

        // 绘制半透明矩形游标
        painter.setPen(QPen(Qt::green, 2));
        painter.setBrush(QColor(0, 255, 0, 50)); // 半透明绿色填充
        painter.drawRect(pixelRect);
        painter.setPen(QPen(Qt::red, 2));
        painter.drawLine(line);

    }

    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            QPointF pos = event->position();
            QRectF absRect = relativeToAbsolute(m_cursorRect);

            // 检查是否点击在调整手柄上
            m_resizeHandle = getResizeHandleAt(pos, absRect);
            if (m_resizeHandle != None) {
                m_resizing = true;
                m_dragStart = pos;
                m_dragStartRect = m_cursorRect;
                return;
            }

            // 检查是否点击在游标内
            if (absRect.contains(pos)) {
                m_dragging = true;
                m_dragStart = pos;
                m_dragStartRect = m_cursorRect;
            }
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        QPointF pos = event->position();
        QRect plotArea = getPlotArea();
        m_cursorRect_abs = relativeToAbsolute(m_cursorRect);

        if( (std::abs(event->position().x()-m_cursorRect_abs.left()) <5) ||(std::abs(event->position().x()-m_cursorRect_abs.right()) <5) )
        {
            // qDebug() << m_cursorRect_abs.left()<<m_cursorRect_abs.right();
            // qDebug() << "Entered:" << event->position();
            setCursor(Qt::SizeHorCursor);
        }
        else
            setCursor(Qt::ArrowCursor);
        if (plotArea.isEmpty()) return;

        double widthFactor = 1.0 / plotArea.width();
        double heightFactor = 1.0 / plotArea.height();

        if (m_dragging) {
            // 计算相对移动量
            double dx = (pos.x() - m_dragStart.x()) * widthFactor;
            double dy = (pos.y() - m_dragStart.y()) * heightFactor;

            // 应用移动并限制边界
            double newX = qBound(0.0, m_dragStartRect.x() + dx, 1.0 - m_dragStartRect.width());
            double newY = qBound(0.0, m_dragStartRect.y() + dy, 1.0 - m_dragStartRect.height());

            setCursorPosition(newX, newY);
            popCursorPosition(newX, newY);

        }
        else if (m_resizing) {
            // 计算相对移动量
            double dx = (pos.x() - m_dragStart.x()) * widthFactor;
            double dy = (pos.y() - m_dragStart.y()) * heightFactor;

            // 根据手柄位置调整大小
            double newX = m_dragStartRect.x();
            double newY = m_dragStartRect.y();
            double newWidth = m_dragStartRect.width();
            double newHeight = m_dragStartRect.height();

            switch (m_resizeHandle) {
            case Left:
                newX = qBound(0.0, m_dragStartRect.x() + dx, m_dragStartRect.right() - 0.01);
                newWidth = qBound(0.01,m_dragStartRect.width() - dx,m_dragStartRect.right());
                // newWidth = qMax(0.01, m_dragStartRect.width() - dx);
                break;
            case Right:
                newWidth = qBound(0.01,m_dragStartRect.width() + dx,1-m_dragStartRect.x());

                break;


            case None:
                break;
            }

            // 应用新大小和位置
            m_cursorRect = QRectF(newX, newY, newWidth, newHeight);
            popCursorSize(m_cursorRect);
            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        Q_UNUSED(event);
        m_dragging = false;
        m_resizing = false;
        m_resizeHandle = None;
    }

    // 处理父窗口大小变化
    bool event(QEvent* event) override {
        switch(event->type()){
            case QEvent::ParentChange:
                syncGeometry(); break;
                return QWidget::event(event);
            case QEvent::HoverEnter:

                return true;
            // case QEvent::HoverLeave:
            //     setCursor(Qt::ArrowCursor);
            //     return true;
            default:
                return QWidget::event(event);
        }

        // if (event->type() == QEvent::ParentChange) {
        //     // 父窗口改变时重新同步
        //     syncGeometry();
        // }
        return QWidget::event(event);
    }

private:
    enum ResizeHandle { None, Left, Right };


    // 获取绘图区域（不包括坐标轴）
    QRect getPlotArea() const {
        if (!m_plot || !m_plot->axisRect())
            return QRect();
        return m_plot->axisRect()->rect();
    }

    // 将相对坐标转换为绝对像素坐标（相对于绘图区域）
    QRectF relativeToAbsolute(const QRectF& relRect) const {
        QRect plotArea = getPlotArea();
        if (plotArea.isEmpty()) return QRectF();

        return QRectF(
            plotArea.left() + relRect.x() * plotArea.width(),
            plotArea.top() + relRect.y() * plotArea.height(),
            relRect.width() * plotArea.width(),
            relRect.height() * plotArea.height()
            );
    }

    // 检查点是否在调整手柄上
    ResizeHandle getResizeHandleAt(const QPointF& pos, const QRectF& rect) const {
        const double handleSize = 8.0;
        // std::abs(event->position().x()-m_cursorRect_abs.left()) <5) ||(std::abs(event->position().x()-m_cursorRect_abs.right()) <5)

        if (std::abs(pos.x()-m_cursorRect_abs.left()) <5)
            return Left;
        if (std::abs(pos.x()-m_cursorRect_abs.right()) <5)
            return Right;

        return None;
    }
    BandType bandtype;
    QCustomPlot* m_plot;
    QRectF m_cursorRect, m_cursorRect_abs; // 存储相对坐标 (0-1)，相对于绘图区域
    bool m_dragging;
    bool m_resizing;
    QPointF m_dragStart;
    QRectF m_dragStartRect;
    ResizeHandle m_resizeHandle;
};

#endif // FFTWITHCURSOR_H
