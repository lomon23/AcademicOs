#ifndef FINANCEFULLPAGE_H
#define FINANCEFULLPAGE_H

#include <QWidget>
#include "FinanceSidebar.h"
#include "FinanceWorkspace.h"

class FinanceFullPage : public QWidget {
    Q_OBJECT
public:
    explicit FinanceFullPage(QWidget *parent = nullptr);

private:
    FinanceSidebar *sidebar;
    FinanceWorkspace *workspace;
};

#endif // FINANCEFULLPAGE_H