#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QTime>

class WelcomeWidget : public QWidget {
    Q_OBJECT
public:
    explicit WelcomeWidget(QWidget *parent = nullptr);

signals:
    void startDayClicked(); // Сигнал, коли натиснули кнопку

private slots:
    void updateTime(); // Оновлює текст (час і привітання)

private:
    QLabel *greetingLabel;
    QLabel *dateLabel;
    QPushButton *startBtn;
    QTimer *timer;

    QString getGreeting() const; // Логіка "Morning/Evening"
};

#endif // WELCOMEWIDGET_H