#ifndef DAILYPAGE_H
#define DAILYPAGE_H

#include <QWidget>
#include <QDate>
#include <QVBoxLayout>
#include <QLabel>

class DailyPage : public QWidget {
    Q_OBJECT
public:
    explicit DailyPage(QWidget *parent = nullptr);

    // Викликається при відкритті сторінки
    void refreshData();

private slots:
    void onPrevDay();
    void onNextDay();
    void onToday();
    void onDateSelected(); // Для вибору через календар (поки заготовка)

private:
    QDate currentDate;
    
    // UI Елементи
    QLabel *dateLabel;
    QVBoxLayout *metricsLayout; // Сюди додаємо спінбокси
    
    void updateDateLabel();
    void buildForm(); // Будує список метрик
};

#endif // DAILYPAGE_H