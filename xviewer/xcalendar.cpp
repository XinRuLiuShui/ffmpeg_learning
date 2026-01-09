#include <QPainter>

#include "xcalendar.h"

XCalendar::XCalendar(QWidget* p):QCalendarWidget(p)
{
}

void XCalendar::paintCell(QPainter* painter, const QRect& rect, const QDate& date) const
{
    // --- 步骤 1: 自定义背景绘制（在基类绘制之前） ---
    // 例如：将周末标记为浅灰色背景
    

    //if (date.dayOfWeek() == Qt::Saturday || date.dayOfWeek() == Qt::Sunday) {
    //    painter->fillRect(rect, QColor("#98F5FF"));
    //}

    //// --- 步骤 2: 调用基类实现（绘制日期文本、选中高亮等）---
    //QCalendarWidget::paintCell(painter, rect, date);

    //

    //// 例如：将当天的日期用红色边框标记出来
    //if (date == QDate::currentDate()) {
    //    painter->setPen(QPen(Qt::red, 2)); // 设置红色画笔，线宽2
    //    painter->drawRect(rect.adjusted(1, 1, -1, -1)); // 绘制矩形边框
    //}

    //// --- 步骤 3: 自定义前景绘制（在基类绘制之后） ---
    //// 例如：在特定日期下面绘制一个小的“事件”点
    //if (date.day() == 15) {
    //    painter->setPen(Qt::NoPen);
    //    painter->setBrush(Qt::blue);
    //    painter->drawEllipse(rect.center(), 3, 3); // 在中心画一个蓝色圆点
    //}


    if (mdate_.find(date) == mdate_.end()) {
        QCalendarWidget::paintCell(painter, rect, date);
        return;
    }


    //设置字体
    auto font = painter->font();
    font.setPixelSize(36);
    painter->setFont(font);



    if (date.dayOfWeek() == Qt::Saturday || date.dayOfWeek() == Qt::Sunday) {
        painter->fillRect(rect, QColor("#FFDAB9"));
    }


    //将当天的日期用红色边框标记出来
    if (date == QDate::currentDate()) {
        painter->setPen(QPen(QColor("#7B68EE"), 2)); // 设置红色画笔，线宽2
        painter->drawRect(rect.adjusted(1, 1, -1, -1)); // 绘制矩形边框
    }
    if (date == selectedDate())
    {
        //painter->setBrush(QColor("#63B8FF"));
        //painter->drawRect(rect);
        painter->fillRect(rect, QColor("#63B8FF"));
    }

    // 如果日期在 mdate_ 中，显示特殊标记或改变背景颜色
    //if (mdate_.find(date) != mdate_.end()) {
    //    // 设置特殊背景颜色
    //    painter->fillRect(rect, QColor("#FFFF00"));  // 有视频的日期，黄色背景

    //    // 可以选择在日期上绘制一个图标或标记（例如小圆点）
    //    //painter->setPen(Qt::NoPen); // 不需要边框
    //    painter->setBrush(Qt::red); // 设置圆点颜色为红色
    //    painter->drawEllipse(rect.center(), 5, 5); // 在日期中心绘制一个红色圆点
    //}

    painter->setPen(QPen(Qt::black, 2));
    painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));

}

