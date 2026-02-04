#ifndef FINANCESMALLWIDGET_H
#define FINANCESMALLWIDGET_H

#include <QWidget>
#include <QLabel>
#include "src/modules/Module.h" // Якщо ти успадковуєшся від ModuleWidget

class FinanceSmallWidget : public QWidget {
    Q_OBJECT
public:
    explicit FinanceSmallWidget(QWidget *parent = nullptr);
    void updateUI(); // Оновити цифри

private:
    QLabel *balanceLabel;
    QLabel *safeLabel;
};

#endif // FINANCESMALLWIDGET_H
