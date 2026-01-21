#ifndef FINANCESMALLWIDGET_H
#define FINANCESMALLWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "FinanceModule.h"

class FinanceSmallWidget : public QWidget {
    Q_OBJECT
public:
    explicit FinanceSmallWidget(FinanceModule *module, QWidget *parent = nullptr);
signals:
    void clicked(); // Сигнал натискання

protected:
    // Переозначаємо клік мишкою
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void updateUI();

private:
    FinanceModule *module;
    QLabel *balanceLabel;
    QVBoxLayout *goalsLayout; // Контейнер для 3-х рядків

    void setupUi();
    QString getColorForType(AllocationType type);
};

#endif // FINANCESMALLWIDGET_H