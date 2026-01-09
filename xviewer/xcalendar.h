#pragma once
#include <set>
#include <QCalendarWidget>

class XCalendar :
    public QCalendarWidget
{
public:
    XCalendar(QWidget* p);
    void paintCell(QPainter* painter, const QRect& rect, const QDate& date) const override;

    void AddDate(QDate d) { mdate_.insert(d); }
    void ClearDate() { mdate_.clear(); }

private:
    std::set<QDate> mdate_; //存放有视频的日期
};

