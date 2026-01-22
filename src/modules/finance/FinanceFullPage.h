#ifndef FINANCEFULLPAGE_H
#define FINANCEFULLPAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QScrollArea>
#include "FinanceModule.h"

class FinanceFullPage : public QWidget {
    Q_OBJECT
public:
    explicit FinanceFullPage(FinanceModule *module, QWidget *parent = nullptr);
    void updateUI();
private slots:
    void onBalanceChanged(const QString &text);
    void onAddClicked();
 
    
private:
    FinanceModule *module;

    // Ліві компоненти
    QLineEdit *balanceInput;
    QLabel *freeBalanceLabel;
    QLabel *allocatedLabel;

    // Праві компоненти
    QWidget *listContainer;
    QVBoxLayout *listLayout;

    void setupUi();
    void updateLabelsOnly();
    void renderAllocations();
    
    QString getColorForType(AllocationType type);
};

#endif // FINANCEFULLPAGE_H