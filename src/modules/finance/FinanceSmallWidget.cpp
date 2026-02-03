

#include "FinanceSmallWidget.h"
#include "FinanceModule.h"
#include <QVBoxLayout>
#include <QFrame>

// УВАГА: Тут НЕ має бути "class FinanceSmallWidget : public QWidget {"
// Тут тільки реалізація методів:

FinanceSmallWidget::FinanceSmallWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QFrame *frame = new QFrame();
    frame->setStyleSheet("background-color: #2A2A2A; border-radius: 10px;");
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    QLabel *title = new QLabel("WALLET SNAPSHOT");
    title->setStyleSheet("color: #888; font-size: 10px; font-weight: bold; letter-spacing: 1px;");

    balanceLabel = new QLabel("0.00 UAH");
    balanceLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");

    QLabel *safeTitle = new QLabel("Safe to Spend:");
    safeTitle->setStyleSheet("color: #F1FA8C; font-size: 10px; margin-top: 5px;");

    safeLabel = new QLabel("0.00 UAH");
    safeLabel->setStyleSheet("color: #F1FA8C; font-size: 14px; font-weight: bold;");

    frameLayout->addWidget(title);
    frameLayout->addWidget(balanceLabel);
    frameLayout->addWidget(safeTitle);
    frameLayout->addWidget(safeLabel);
    frameLayout->addStretch();

    layout->addWidget(frame);

    // Підписуємось на оновлення
    connect(&FinanceModule::instance(), &FinanceModule::dataChanged, this, &FinanceSmallWidget::updateUI);
    updateUI();
}

void FinanceSmallWidget::updateUI() {
    auto &fin = FinanceModule::instance();
    
    double total = fin.getTotalBalanceInUAH();
    double safe = fin.getDisposableBalance();

    balanceLabel->setText(QString::number(total, 'f', 2) + " UAH");
    safeLabel->setText(QString::number(safe, 'f', 2) + " UAH");
}