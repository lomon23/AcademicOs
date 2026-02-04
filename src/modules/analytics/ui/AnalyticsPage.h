#ifndef ANALYTICSPAGE_H
#define ANALYTICSPAGE_H

#include <QWidget>
#include <QGridLayout>

class AnalyticsPage : public QWidget {
    Q_OBJECT
public:
    explicit AnalyticsPage(QWidget *parent = nullptr);

private:
    void setupLayout();
};

#endif // ANALYTICSPAGE_H