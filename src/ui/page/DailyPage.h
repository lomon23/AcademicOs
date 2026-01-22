#ifndef DAILYPAGE_H
#define DAILYPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QDate>
#include "../../core/MetricManager.h"

class DailyPage : public QWidget {
    Q_OBJECT
public:
    explicit DailyPage(QWidget *parent = nullptr);
    
    void prepareForShow(); 
    
    // НОВЕ: Встановлюємо баланс з гаманця перед показом
    void setWalletBalance(double amount); 

signals:
    // НОВЕ: Сигнал, що ми вирахували витрати (щоб оновити Гаманець)
    void walletCorrection(double spentAmount); 
    void finished();

private slots:
    void onSaveClicked();

private:
    QVBoxLayout *mainLayout;
    QFormLayout *formLayout;
    QLabel *dateLabel;
    double currentBalance = 0.0;
    // Структура для зберігання посилань на створені віджети
    struct InputField {
        MetricType type;
        QWidget *widget; // Вказівник на сам віджет (QSpinBox, QSlider тощо)
    };
    
    // Мапа: ID метрики -> Поле вводу
    QMap<QString, InputField> inputs; 

    void setupUi();
    void buildForm(); // Генерує поля на основі MetricManager
};

#endif // DAILYPAGE_H