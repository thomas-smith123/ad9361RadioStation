// #include "fftwithcursor.h"

// fftwithcursor::fftwithcursor(QCustomPlot* plotWidget, QWidget *parent)
//     : QWidget{parent}
// {
//     setAttribute(Qt::WA_TransparentForMouseEvents); // 穿透鼠标事件
//     setAttribute(Qt::WA_NoSystemBackground);        // 无背景
//     m_cursorRect = QRectF(0.4, 0.4, 0.2, 0.2); // 相对坐标 (x,y,w,h) 范围 0-1
//     m_plot = plotWidget;
//     syncGeometry();
// }
// void fftwithcursor::setCursorPosition(double x, double y) {
//     // 保持游标尺寸不变，只改变位置
//     m_cursorRect.moveTo(x, y);
//     update();
// }

// // 显示/隐藏游标
//  QPointF fftwithcursor::cursorPosition()const{
//     return m_cursorRect.topLeft();
// }

// void fftwithcursor::syncGeometry() {
//     if (m_plot) {
//         setGeometry(m_plot->geometry());
//         raise();  // 确保在最上层
//         update();
//     }
// }
// // QRectF fftwithcursor::relativeToAbsolute(const QRectF& relRect) {
// //     return QRectF(
// //         relRect.x() * width(),
// //         relRect.y() * height(),
// //         relRect.width() * width(),
// //         relRect.height() * height()
// //         );
// // }
// void fftwithcursor::paintEvent(QPaintEvent*)  {
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);

//     // 计算实际像素位置
//     QRectF pixelRect = relativeToAbsolute(m_cursorRect);

//     // 绘制半透明矩形游标
//     painter.setPen(QPen(Qt::green, 2));
//     painter.setBrush(QColor(0, 255, 0, 50)); // 半透明绿色填充
//     painter.drawRect(pixelRect);
// }


// void fftwithcursor::mousePressEvent(QMouseEvent* event){
//     if (event->button() == Qt::LeftButton) {
//         // 检查是否点击在游标内
//         QRectF absRect = relativeToAbsolute(m_cursorRect);
//         if (absRect.contains(event->position())) {
//             m_dragging = true;
//             m_dragOffset = event->position() - absRect.topLeft();
//         }
//     }
// }

// void fftwithcursor::mouseMoveEvent(QMouseEvent* event)
// {
//     if (m_dragging) {
//         // 计算新的位置（限制在控件范围内）
//         QPointF newPos = event->position() - m_dragOffset;

//         // 转换为相对坐标 (0-1)
//         double relX = qBound(0.0, newPos.x() / width(), 1.0 - m_cursorRect.width());
//         double relY = qBound(0.0, newPos.y() / height(), 1.0 - m_cursorRect.height());

//         setCursorPosition(relX, relY);
//     }
// }

// void fftwithcursor::mouseReleaseEvent(QMouseEvent* event)
// {
//     Q_UNUSED(event);
//     m_dragging = false;
// }
