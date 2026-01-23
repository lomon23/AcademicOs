#ifndef CALENDARPAGE_H
#define CALENDARPAGE_H

#include <QWidget>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QLabel>

class CalendarPage : public QWidget {
    Q_OBJECT

public:
    explicit CalendarPage(QWidget *parent = nullptr);

private:
    QCalendarWidget *calendar;
    void setupUI();
};

#endif // CALENDARPAGE_H