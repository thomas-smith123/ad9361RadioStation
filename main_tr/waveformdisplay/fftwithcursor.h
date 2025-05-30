#ifndef FFTWITHCURSOR_H
#define FFTWITHCURSOR_H

#include <QWidget>
#include "QPainter"
#include "QMouseEvent"
#include <QObject>
#include <QRectF>
#include <QColor>
#include "../qcustomplot/qcustomplot.h"
// 简洁的游标覆盖控件
class fftwithcursor : public QWidget {
public:
    explicit fftwithcursor(QCustomPlot* plotWidget, QWidget* parent = nullptr)
        : QWidget(parent), m_plot(plotWidget), m_dragging(false), m_resizing(false) {
        // 设置透明属性
        setAttribute(Qt::WA_TransparentForMouseEvents, false); // 需要接收鼠标事件
        setStyleSheet("background-color: transparent;");

        // 初始游标位置和大小（相对坐标，相对于绘图区域）
        m_cursorRect = QRectF(0.4, 0.4, 0, 1); // x, y, width, height (0-1)

        // 初始同步
        syncGeometry();
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
        update();
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
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 获取绘图区域（不包括坐标轴）
        QRect plotArea = getPlotArea();
        if (plotArea.isEmpty()) return;

        // 计算实际像素位置
        QRectF pixelRect = relativeToAbsolute(m_cursorRect);

        // 绘制半透明矩形游标
        painter.setPen(QPen(Qt::green, 2));
        painter.setBrush(QColor(0, 255, 0, 50)); // 半透明绿色填充
        painter.drawRect(pixelRect);

        // 在四个角绘制调整大小的手柄
        painter.setBrush(Qt::red);

        // 左上角手柄
        painter.drawRect(QRectF(pixelRect.topLeft() - QPointF(4, 4), QSizeF(8, 8)));
        // 右上角手柄
        painter.drawRect(QRectF(pixelRect.topRight() - QPointF(4, 4), QSizeF(8, 8)));
        // 左下角手柄
        painter.drawRect(QRectF(pixelRect.bottomLeft() - QPointF(4, 4), QSizeF(8, 8)));
        // 右下角手柄
        painter.drawRect(QRectF(pixelRect.bottomRight() - QPointF(4, 4), QSizeF(8, 8)));
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
            case TopLeft:
                newX = qBound(0.0, m_dragStartRect.x() + dx, m_dragStartRect.right() - 0.01);
                newY = qBound(0.0, m_dragStartRect.y() + dy, m_dragStartRect.bottom() - 0.01);
                newWidth = qMax(0.01, m_dragStartRect.width() - dx);
                newHeight = qMax(0.01, m_dragStartRect.height() - dy);
                break;

            case TopRight:
                newY = qBound(0.0, m_dragStartRect.y() + dy, m_dragStartRect.bottom() - 0.01);
                newWidth = qMax(0.01, m_dragStartRect.width() + dx);
                newHeight = qMax(0.01, m_dragStartRect.height() - dy);
                break;

            case BottomLeft:
                newX = qBound(0.0, m_dragStartRect.x() + dx, m_dragStartRect.right() - 0.01);
                newWidth = qMax(0.01, m_dragStartRect.width() - dx);
                newHeight = qMax(0.01, m_dragStartRect.height() + dy);
                break;

            case BottomRight:
                newWidth = qMax(0.01, m_dragStartRect.width() + dx);
                newHeight = qMax(0.01, m_dragStartRect.height() + dy);
                break;

            case None:
                break;
            }

            // 应用新大小和位置
            m_cursorRect = QRectF(newX, newY, newWidth, newHeight);
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
        if (event->type() == QEvent::ParentChange) {
            // 父窗口改变时重新同步
            syncGeometry();
        }
        return QWidget::event(event);
    }

private:
    enum ResizeHandle { None, TopLeft, TopRight, BottomLeft, BottomRight };

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

        // 检查四个角
        QRectF topLeftHandle(rect.topLeft() - QPointF(handleSize/2, handleSize/2),
                             QSizeF(handleSize, handleSize));
        if (topLeftHandle.contains(pos)) return TopLeft;

        QRectF topRightHandle(rect.topRight() - QPointF(handleSize/2, handleSize/2),
                              QSizeF(handleSize, handleSize));
        if (topRightHandle.contains(pos)) return TopRight;

        QRectF bottomLeftHandle(rect.bottomLeft() - QPointF(handleSize/2, handleSize/2),
                                QSizeF(handleSize, handleSize));
        if (bottomLeftHandle.contains(pos)) return BottomLeft;

        QRectF bottomRightHandle(rect.bottomRight() - QPointF(handleSize/2, handleSize/2),
                                 QSizeF(handleSize, handleSize));
        if (bottomRightHandle.contains(pos)) return BottomRight;

        return None;
    }

    QCustomPlot* m_plot;
    QRectF m_cursorRect; // 存储相对坐标 (0-1)，相对于绘图区域
    bool m_dragging;
    bool m_resizing;
    QPointF m_dragStart;
    QRectF m_dragStartRect;
    ResizeHandle m_resizeHandle;
};

#endif // FFTWITHCURSOR_H
