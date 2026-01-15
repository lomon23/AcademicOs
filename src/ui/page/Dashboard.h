#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QGridLayout>

class Dashboard :public QWidget {
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);

private:
    QGridLayout *mainLayout;  
};


#endif