#ifndef FINANCESIDEBAR_H
#define FINANCESIDEBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "FinanceModule.h"

class FinanceSidebar : public QWidget {
    Q_OBJECT
public:
    explicit FinanceSidebar(QWidget *parent = nullptr);
    void refreshData(); // Оновити цифри

private:
    QVBoxLayout *mainLayout;
    QVBoxLayout *accountsLayout; // Сюди будемо додавати картки рахунків

    // Основні лейбли
    QLabel *totalBalanceLabel;
    QLabel *disposableLabel;

    void setupLayout();
    void buildAccountCard(const WalletAccount &acc); // Хелпер для малювання картки
};

#endif // FINANCESIDEBAR_H